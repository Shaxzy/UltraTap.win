#include "..\includes.h"

#include "hooks.h"
#include "../UTILS/interfaces.h"
#include "../UTILS/offsets.h"
#include "../UTILS/NetvarHookManager.h"
#include "../UTILS/render.h"

#include "../SDK/CInput.h"
#include "../SDK/IClient.h"
#include "../SDK/CPanel.h"
#include "../SDK/ConVar.h"
#include "../SDK/CGlowObjectManager.h"
#include "../SDK/IEngine.h"
#include "../SDK/CTrace.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/RecvData.h"
#include "../SDK/CBaseAnimState.h"
#include "../SDK/ModelInfo.h"
#include "../SDK/ModelRender.h"
#include "../SDK/RenderView.h"
#include "../SDK/CTrace.h"
#include "../SDK/CViewSetup.h"
#include "../SDK/CGlobalVars.h"
#include "../SDK/CPrediction.h"

#include "../FEATURES/Movement.h"
#include "../FEATURES/Visuals.h"
#include "../FEATURES/Chams.h"
#include "../FEATURES/AntiAim.h"
#include "../FEATURES/Aimbot.h"
#include "../FEATURES/Resolver.h"
#include "../FEATURES/Backtracking.h"
#include "../FEATURES/FakeWalk.h"
#include "../FEATURES/FakeLag.h"
#include "../FEATURES/EnginePred.h"
#include "../FEATURES/EventListener.h"
#include "../FEATURES/GrenadePrediction.h"
#include "../FEATURES/Legitbot.h"
#include "../FEATURES/Flashlight.h"
#include "../FEATURES/InGameLogger.h"

#include "../MENU/TMenu.h"

#include <intrin.h>

//--- Other Globally Used Variables ---///
static bool tick = false;
Vector vecAimPunch, vecViewPunch;
Vector* pAimPunch = nullptr;
Vector* pViewPunch = nullptr;

//--- Declare Signatures and Patterns Here ---///
static auto CAM_THINK = UTILS::FindSignature("client_panorama.dll", "85 C0 75 30 38 86");
static auto linegoesthrusmoke = UTILS::FindPattern("client_panorama.dll", (PBYTE)"\x55\x8B\xEC\x83\xEC\x08\x8B\x15\x00\x00\x00\x00\x0F\x57\xC0", "xxxxxxxx????xxx");
std::string sPanel = ("FocusOverlayPanel");
int CC_c1 = 0;
int CC_c2 = 0;
void ClantagChanger()
{
	auto SetClanTag = [](const char* tag, const char* name)
	{
		typedef void(__fastcall* SetClanTagFn)(const char*, const char*);
		static auto set_clan_tag = reinterpret_cast<SetClanTagFn>(UTILS::FindPattern(("engine.dll"), (PBYTE)("\x53\x56\x57\x8B\xDA\x8B\xF9\xFF\x15\x00\x00\x00\x00\x6A\x24\x8B\xC8\x8B\x30"), ("xxxxxxxxx????xxxxxx")));
		set_clan_tag(tag, name);
	};
	auto TextMarquee = [](std::string text, int speed) -> std::string
	{
		if (!text.size() || !speed)
			return text;

		speed = speed % text.size();
		for (int i = 0; i < speed; i++)
		{
			char back = text.back();
			text.pop_back();
			text.insert(text.begin(), back);
		}
		return text;
	};
	auto TextShift = [](const char* text, float speed) -> const char*

	{
		static constexpr float num_spaces = 10;
		const int str_len = strlen(text);

		if (!speed || str_len <= 0)
			return text;

		const int half_size = fmod(speed / num_spaces, str_len);
		char final_text[128];
		memcpy(final_text, text, half_size);

		int i = 0;
		for (; i < num_spaces - fmod(speed, num_spaces); i++)
			final_text[half_size + i] = ' ';

		final_text[half_size + i] = text[half_size];

		for (i = half_size + i + 1; i < str_len + half_size + num_spaces; i++)
			final_text[i] = ' ';

		final_text[i] = '\0';

		return final_text;
	};

	if (!SETTINGS::settings.misc_enable_clantag)
		return;

	static size_t lastTime = 0;
	if (GetTickCount() > lastTime)
	{
		switch (SETTINGS::settings.misc_clantag_style)
		{
		case 0: /// static
			SetClanTag(SETTINGS::settings.misc_clantag_text, "");
			break;
		case 1: /// marquee
			SetClanTag(TextMarquee(SETTINGS::settings.misc_clantag_text, lastTime * SETTINGS::settings.misc_clantag_speed).c_str(), "");
			break;
		case 2: /// shift
			SetClanTag(TextShift(SETTINGS::settings.misc_clantag_text, lastTime * SETTINGS::settings.misc_clantag_speed), "");
			break;
		}

		lastTime = GetTickCount() + 500;
	}
}

const std::string currentDateTime() {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%Y | %d | %X", &tstruct);

	return buf;
}

void auto_revolver(SDK::CUserCmd* cmd)
{
	auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
	if (!local_player || local_player->GetHealth() <= 0)
		return;

	auto weapon = reinterpret_cast<SDK::CBaseWeapon*>(INTERFACES::ClientEntityList->GetClientEntity(local_player->GetActiveWeaponIndex()));
	if (!weapon || weapon->GetItemDefenitionIndex() != SDK::WEAPON_REVOLVER)
		return;

	static float delay = 0.f;
	if (delay < 0.15f)
	{
		delay += INTERFACES::Globals->interval_per_tick;
		cmd->buttons |= IN_ATTACK;
	}
	else
		delay = 0.f;
}
namespace HOOKS
{
	CreateMoveFn original_create_move;
	PaintTraverseFn original_paint_traverse;
	PaintFn original_paint;
	FrameStageNotifyFn original_frame_stage_notify;
	DrawModelExecuteFn original_draw_model_execute;
	SceneEndFn original_scene_end;
	TraceRayFn original_trace_ray;
	OverrideViewFn original_override_view;
	RenderViewFn original_render_view;
	SvCheatsGetBoolFn original_get_bool;
	GetViewmodelFOVFn original_viewmodel_fov;

	VMT::VMTHookManager iclient_hook_manager;
	VMT::VMTHookManager panel_hook_manager;
	VMT::VMTHookManager paint_hook_manager;
	VMT::VMTHookManager model_render_hook_manager;
	VMT::VMTHookManager scene_end_hook_manager;
	VMT::VMTHookManager render_view_hook_manager;
	VMT::VMTHookManager trace_hook_manager;
	VMT::VMTHookManager net_channel_hook_manager;
	VMT::VMTHookManager override_view_hook_manager;
	VMT::VMTHookManager input_table_manager;
	VMT::VMTHookManager get_bool_manager;
	float spread;
	bool __stdcall HookedCreateMove(float sample_input_frametime, SDK::CUserCmd* cmd)
	{
		if (!cmd || cmd->command_number == 0)
			return false;

		uintptr_t* FPointer; __asm { MOV FPointer, EBP }
		byte* SendPacket = (byte*)(*FPointer - 0x1C);
		if (!SendPacket) return false;
		float get_speed;
		auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
		if (!local_player) return false;

		GLOBAL::should_send_packet = *SendPacket;
		GLOBAL::originalCMD = *cmd;
		if (INTERFACES::Engine->IsConnected() && INTERFACES::Engine->IsInGame())
		{
			GrenadePrediction::instance().Tick(cmd->buttons);
			if (SETTINGS::settings.autozeus_bool)
				aimbot->autozeus(cmd);
			if (!GetAsyncKeyState(0x56))
				fakelag->do_fakelag(cmd);
			if (!GetAsyncKeyState(VK_ADD))
				slowwalk->SlowWalk(cmd);
			if (SETTINGS::settings.bhop_bool)
				movement->bunnyhop(cmd);
			if (SETTINGS::settings.strafe_bool)
				movement->autostrafer(cmd);
			if (SETTINGS::settings.aim_type == 0 && SETTINGS::settings.aim_bool)
			{
				aimbot->run_aimbot(cmd);


			}

			if (SETTINGS::settings.aa_bool)
			{
				antiaim->do_antiaim(cmd);
				antiaim->fix_movement(cmd);
			}
		}

		if (SETTINGS::settings.duck_bool)
			movement->duckinair(cmd);

		{


			if (SETTINGS::settings.misc_enable_clantag)
				ClantagChanger();

			if (!GLOBAL::should_send_packet)
				GLOBAL::real_angles = cmd->viewangles;
			else
			{
			}

		}
		*SendPacket = GLOBAL::should_send_packet;
		cmd->move = antiaim->fix_movement(cmd, GLOBAL::originalCMD);
		if (SETTINGS::settings.aa_pitch < 2 || SETTINGS::settings.aa_pitch1_type < 2 || SETTINGS::settings.aa_pitch2_type < 2)
			UTILS::ClampLemon(cmd->viewangles);
		return false;
	}
	void __stdcall HookedPaintTraverse(int VGUIPanel, bool ForceRepaint, bool AllowForce)
	{
		std::string panel_name = INTERFACES::Panel->GetName(VGUIPanel);
		if (panel_name == "HudZoom" && SETTINGS::settings.scope_bool) return;

		if (panel_name == "MatSystemTopPanel")
		{
			int screen_width, screen_height;
			INTERFACES::Engine->GetScreenSize(screen_width, screen_height);

			auto DrawInject = [screen_width, screen_height]()
			{
				static constexpr float intro_length_time = 3.f, intro_max_height_offset = -25.f, intro_max_width_offset = -15.f;

				const float current_time = GetTickCount() * 0.001f, time_delta = current_time - GLOBAL::cheat_start_time;
				if (time_delta < intro_length_time)
				{
					static constexpr char BXDYAIMWIN_text[] = "     U L T R A T  A P.W I N     ";

					const int text_width = RENDER::GetTextSize(FONTS::welcome_font, BXDYAIMWIN_text).x, text_len = strlen(BXDYAIMWIN_text);
					const int center_width = screen_width * 0.5, center_height = screen_height * 0.5;
					const int current_letter = ((time_delta / intro_length_time) * text_len) + 1;
					const float time_per_letter = intro_length_time / static_cast<float>(text_len);
					for (int i = 0; i < text_len; i++)
					{
						if (i >= current_letter)
							continue;

						const float frac = ((fmod(time_delta, time_per_letter) * 0.5f + (i == current_letter - 2 ? time_per_letter * 0.5f : 0)) / time_per_letter);

						int alpha = 0;
						if (current_letter > i + 2)
							alpha = 255;
						else
							alpha = frac * 255.f;

						const int cur_width = center_width + (text_width * 0.5f) - (text_width / static_cast<float>(text_len)) * static_cast<float>(text_len - i);
						if (i == current_letter - 1 || i == current_letter - 2)
						{
							const float frac_2 = (frac > 0.5f ? 1.f - frac : frac) * 2.f;
							RENDER::DrawF(cur_width + (frac_2 * intro_max_width_offset), center_height + (frac_2 * intro_max_height_offset), FONTS::welcome_font, false, true, CColor(6, 137, 255, alpha), std::string(1, BXDYAIMWIN_text[i]));
						}
						else
							RENDER::DrawF(cur_width, center_height, FONTS::welcome_font, false, true, CColor(225, 137, 6, alpha), std::string(1, BXDYAIMWIN_text[i]));
					}
				}
			};

			DrawInject(); // draw intro

			TGUI::MENU::Do(); // draw menu

			in_game_logger.Do(); // draw event logs

								 // draw watermark


		}

		if (panel_name == "FocusOverlayPanel")
		{
			if (FONTS::ShouldReloadFonts())
				FONTS::InitFonts();

			if (INTERFACES::Engine->IsConnected() && INTERFACES::Engine->IsInGame())
			{
				GrenadePrediction::instance().Paint();

				auto matpostprocess = INTERFACES::cvar->FindVar("mat_postprocess_enable");
				matpostprocess->fnChangeCallback = 0;
				matpostprocess->SetValue(SETTINGS::settings.matpostprocessenable);

				visuals->ModulateWorld();

				if (SETTINGS::settings.esp_bool)
				{
					visuals->Draw();
					visuals->ClientDraw();
				}
				Flashlight.RunFrame();
			}

			UTILS::INPUT::input_handler.Update();
		}

		original_paint_traverse(INTERFACES::Panel, VGUIPanel, ForceRepaint, AllowForce);



		const char* pszPanelName = INTERFACES::Panel->GetName(VGUIPanel);

		if (!strstr(pszPanelName, sPanel.data()))
			return;


		INTERFACES::Panel->SetMouseInputEnabled(VGUIPanel, menu_open);





	}
	void __fastcall HookedFrameStageNotify(void* ecx, void* edx, int stage)
	{
		auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
		if (!local_player) return;
		Vector vecAimPunch, vecViewPunch;
		Vector* pAimPunch = nullptr; Vector* pViewPunch = nullptr;

		switch (stage)
		{
		case FRAME_NET_UPDATE_POSTDATAUPDATE_START:
			if (INTERFACES::Engine->IsConnected() && INTERFACES::Engine->IsInGame())
			{
				for (int i = 1; i <= 65; i++)
				{
					auto entity = INTERFACES::ClientEntityList->GetClientEntity(i);
					if (!entity) continue;

					bool is_local_player = entity == local_player;
					bool is_teammate = local_player->GetTeam() == entity->GetTeam() && !is_local_player;

					if (is_local_player) continue;
					if (is_teammate) continue;
					if (entity->GetHealth() <= 0) continue;
					if (entity->GetIsDormant()) continue;

					if (SETTINGS::settings.aim_type == 0 && SETTINGS::settings.resolve_bool)
						resolver->resolve(entity);
				}
			} break;
		case FRAME_RENDER_START:
			if (INTERFACES::Engine->IsConnected() && INTERFACES::Engine->IsInGame())
			{
				if (in_tp)
				{
					SDK::CBaseAnimState* animstate = local_player->GetAnimState();

					if (!animstate)
						return;

					if (animstate->m_bInHitGroundAnimation)
						*(Vector*)((DWORD)local_player + 0x31C8) = Vector(1.0f, GLOBAL::real_angles.y, 0.f);
					else
						*(Vector*)((DWORD)local_player + 0x31C8) = Vector(GLOBAL::real_angles.x, GLOBAL::real_angles.y, 0.f);

					INTERFACES::pPrediction->SetLocalViewAngles(GLOBAL::real_angles);
					local_player->UpdateClientSideAnimation();
					INTERFACES::pPrediction->SetLocalViewAngles(GLOBAL::fake_angles);

				}
				for (int i = 1; i <= 65; i++)
				{
					auto entity = INTERFACES::ClientEntityList->GetClientEntity(i);
					if (!entity) continue;
					if (entity == local_player) continue;

					*(int*)((uintptr_t)entity + 0xA30) = INTERFACES::Globals->framecount;
					*(int*)((uintptr_t)entity + 0xA28) = 0;
				}
			} break;

		case FRAME_NET_UPDATE_START:
			if (INTERFACES::Engine->IsConnected() && INTERFACES::Engine->IsInGame())
			{
				if (SETTINGS::settings.beam_bool)
					visuals->DrawBulletBeams();
			} break;
		case FRAME_NET_UPDATE_END:
		{}
		break;
		}
		original_frame_stage_notify(ecx, stage);
	}
	void __fastcall HookedDrawModelExecute(void* ecx, void* edx, SDK::IMatRenderContext* context, const SDK::DrawModelState_t& state, const SDK::ModelRenderInfo_t& render_info, matrix3x4_t* matrix)
	{
		if (INTERFACES::Engine->IsConnected() && INTERFACES::Engine->IsInGame())
		{
			for (int i = 1; i < 65; i++)
			{
				auto entity = INTERFACES::ClientEntityList->GetClientEntity(i);
				auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());

				if (!entity)
					continue;

				if (!local_player)
					continue;
				std::string ModelName = INTERFACES::ModelInfo->GetModelName(render_info.pModel);

				if (ModelName.find("v_sleeve") != std::string::npos)
				{
					SDK::IMaterial* material = INTERFACES::MaterialSystem->FindMaterial(ModelName.c_str(), TEXTURE_GROUP_MODEL);
					if (!material) return;
					material->SetMaterialVarFlag(SDK::MATERIAL_VAR_NO_DRAW, true);
					INTERFACES::ModelRender->ForcedMaterialOverride(material);
					if (entity && entity->GetIsScoped() && in_tp && entity == local_player && !SETTINGS::settings.glow_bool) {
						INTERFACES::RenderView->SetBlend(0.4);
					}

				}
			}
		}
		original_draw_model_execute(ecx, context, state, render_info, matrix);
	}
	void __fastcall HookedSceneEnd(void* ecx, void* edx)
	{
		original_scene_end(ecx);
		static SDK::IMaterial* ignorez = chams->CreateMaterialBasic(true, true, false);
		static SDK::IMaterial* notignorez = chams->CreateMaterialBasic(false, true, false);
		static SDK::IMaterial* ignorez_metallic = chams->CreateMaterialMetallic(true, true, false);
		static SDK::IMaterial* notignorez_metallic = chams->CreateMaterialMetallic(false, true, false);

		if (INTERFACES::Engine->IsConnected() && INTERFACES::Engine->IsInGame())
		{
			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player) return;
			CColor color = SETTINGS::settings.glow_col, colorTeam = SETTINGS::settings.teamglow_color, colorlocal = SETTINGS::settings.glowlocal_col;
			for (int i = 1; i < 65; i++)
			{
				auto entity = INTERFACES::ClientEntityList->GetClientEntity(i);

				if (!entity) continue;
				if (!local_player) continue;

				bool is_local_player = entity == local_player;
				bool is_teammate = local_player->GetTeam() == entity->GetTeam() && !is_local_player;
				auto ignorezmaterial = SETTINGS::settings.chamstype == 0 ? ignorez_metallic : ignorez;
				auto notignorezmaterial = SETTINGS::settings.chamstype == 0 ? notignorez_metallic : notignorez;
				if (is_local_player)
				{
					switch (SETTINGS::settings.localchams)
					{
						case 0: continue; break;
						case 1:
							local_player->SetAbsOrigin(GLOBAL::FakePosition);
							local_player->DrawModel(0x1, 255);
							local_player->SetAbsOrigin(local_player->GetAbsOrigin());
							continue; break;
						case 2:
							notignorezmaterial->ColorModulate(SETTINGS::settings.localchams_col);
							INTERFACES::ModelRender->ForcedMaterialOverride(notignorezmaterial);
							local_player->DrawModel(0x1, 255);
							INTERFACES::ModelRender->ForcedMaterialOverride(nullptr);
							continue;  break;
						case 3:
							notignorezmaterial->ColorModulate(SETTINGS::settings.localchams_col);
							INTERFACES::ModelRender->ForcedMaterialOverride(notignorezmaterial);
							local_player->SetAbsOrigin(GLOBAL::FakePosition);
							local_player->DrawModel(0x1, 255);
							local_player->SetAbsOrigin(local_player->GetAbsOrigin());
							INTERFACES::ModelRender->ForcedMaterialOverride(nullptr);
							continue; break;						
						case 4:
							Vector OrigAng;
							OrigAng = local_player->GetEyeAngles();
							//---p100 fake angle detector---//
							local_player->SetAngle2(Vector(0, local_player->GetLowerBodyYaw(), 0));
							//---End---//
							INTERFACES::RenderView->SetColorModulation(SETTINGS::settings.localchams_col); //color thingy on the menu//
							INTERFACES::ModelRender->ForcedMaterialOverride(notignorezmaterial);
							local_player->SetAbsOrigin(GLOBAL::FakePosition);
							local_player->DrawModel(0x1, 255);
							INTERFACES::ModelRender->ForcedMaterialOverride(nullptr);
							local_player->SetAngle2(OrigAng);
							local_player->SetAbsOrigin(local_player->GetAbsOrigin());
							continue; break;
					}
				}

				if (entity->GetHealth() <= 0) continue;
				if (entity->GetIsDormant())	continue;
				if (entity->GetClientClass()->m_ClassID != 38) continue;

				if (is_teammate)
				{
					if (entity && SETTINGS::settings.chamsteam == 2)
					{
						ignorezmaterial->ColorModulate(SETTINGS::settings.teaminvis_color);
						INTERFACES::ModelRender->ForcedMaterialOverride(ignorezmaterial);
						entity->DrawModel(0x1, 255);

						notignorezmaterial->ColorModulate(SETTINGS::settings.teamvis_color);
						INTERFACES::ModelRender->ForcedMaterialOverride(notignorezmaterial);
						entity->DrawModel(0x1, 255);

						INTERFACES::ModelRender->ForcedMaterialOverride(nullptr);
					}
					else if (entity && SETTINGS::settings.chamsteam == 1)
					{
						notignorezmaterial->ColorModulate(SETTINGS::settings.teamvis_color);
						INTERFACES::ModelRender->ForcedMaterialOverride(notignorezmaterial);
						entity->DrawModel(0x1, 255);

						INTERFACES::ModelRender->ForcedMaterialOverride(nullptr);
					} continue;
				}
				else if (is_teammate && SETTINGS::settings.chamsteam)
					continue;

				if (entity && SETTINGS::settings.chams_type == 2)
				{
					ignorezmaterial->ColorModulate(SETTINGS::settings.imodel_col);
					INTERFACES::ModelRender->ForcedMaterialOverride(ignorezmaterial);
					entity->DrawModel(0x1, 255);

					notignorezmaterial->ColorModulate(SETTINGS::settings.vmodel_col);
					INTERFACES::ModelRender->ForcedMaterialOverride(notignorezmaterial);
					entity->DrawModel(0x1, 255);

					INTERFACES::ModelRender->ForcedMaterialOverride(nullptr);
				}
				else if (entity && SETTINGS::settings.chams_type == 1)
				{
					notignorezmaterial->ColorModulate(SETTINGS::settings.vmodel_col);
					INTERFACES::ModelRender->ForcedMaterialOverride(notignorezmaterial);
					entity->DrawModel(0x1, 255);

					INTERFACES::ModelRender->ForcedMaterialOverride(nullptr);
				}
			}

			for (auto i = 0; i < INTERFACES::GlowObjManager->GetSize(); i++)
			{
				auto &glowObject = INTERFACES::GlowObjManager->m_GlowObjectDefinitions[i];
				auto entity = reinterpret_cast<SDK::CBaseEntity*>(glowObject.m_pEntity);

				if (!entity) continue;
				if (!local_player) continue;

				if (glowObject.IsUnused()) continue;

				bool is_local_player = entity == local_player;
				bool is_teammate = local_player->GetTeam() == entity->GetTeam() && !is_local_player;

				if (is_local_player && in_tp && SETTINGS::settings.glowlocal)
				{
					glowObject.m_nGlowStyle = SETTINGS::settings.glowstylelocal;
					glowObject.m_flRed = colorlocal.RGBA[0] / 255.0f;
					glowObject.m_flGreen = colorlocal.RGBA[1] / 255.0f;
					glowObject.m_flBlue = colorlocal.RGBA[2] / 255.0f;
					glowObject.m_flAlpha = colorlocal.RGBA[3] / 255.0f;
					glowObject.m_bRenderWhenOccluded = true;
					glowObject.m_bRenderWhenUnoccluded = false;
					continue;
				}
				else if (!SETTINGS::settings.glowlocal && is_local_player)
					continue;

				if (entity->GetHealth() <= 0) continue;
				if (entity->GetIsDormant())	continue;
				if (entity->GetClientClass()->m_ClassID != 38) continue;

				if (is_teammate && SETTINGS::settings.glowteam)
				{
					glowObject.m_nGlowStyle = SETTINGS::settings.glowstyle; //0;
					glowObject.m_flRed = colorTeam.RGBA[0] / 255.0f;
					glowObject.m_flGreen = colorTeam.RGBA[1] / 255.0f;
					glowObject.m_flBlue = colorTeam.RGBA[2] / 255.0f;
					glowObject.m_flAlpha = colorTeam.RGBA[3] / 255.0f;
					glowObject.m_bRenderWhenOccluded = true;
					glowObject.m_bRenderWhenUnoccluded = false;
					continue;
				}
				else if (is_teammate && !SETTINGS::settings.glowteam)
					continue;

				if (SETTINGS::settings.glowenable)
				{
					glowObject.m_nGlowStyle = SETTINGS::settings.glowstyle;//0;
					glowObject.m_flRed = color.RGBA[0] / 255.0f;
					glowObject.m_flGreen = color.RGBA[1] / 255.0f;
					glowObject.m_flBlue = color.RGBA[2] / 255.0f;
					glowObject.m_flAlpha = color.RGBA[3] / 255.0f;
					glowObject.m_bRenderWhenOccluded = true;
					glowObject.m_bRenderWhenUnoccluded = false;
				}
			}

			if (SETTINGS::settings.smoke_bool)
			{
				std::vector<const char*> vistasmoke_wireframe =  { "particle/vistasmokev1/vistasmokev1_smokegrenade" };

				std::vector<const char*> vistasmoke_nodraw =
				{
					"particle/vistasmokev1/vistasmokev1_fire",
					"particle/vistasmokev1/vistasmokev1_emods",
					"particle/vistasmokev1/vistasmokev1_emods_impactdust",
				};

				for (auto mat_s : vistasmoke_wireframe)
				{
					SDK::IMaterial* mat = INTERFACES::MaterialSystem->FindMaterial(mat_s, TEXTURE_GROUP_OTHER);
					mat->SetMaterialVarFlag(SDK::MATERIAL_VAR_WIREFRAME, true); //wireframe
				}

				for (auto mat_n : vistasmoke_nodraw)
				{
					SDK::IMaterial* mat = INTERFACES::MaterialSystem->FindMaterial(mat_n, TEXTURE_GROUP_OTHER);
					mat->SetMaterialVarFlag(SDK::MATERIAL_VAR_NO_DRAW, true);
				}

				static auto smokecout = *(DWORD*)(linegoesthrusmoke + 0x8);
				*(int*)(smokecout) = 0;
			}
		}
	}
	void __fastcall HookedOverrideView(void* ecx, void* edx, SDK::CViewSetup* pSetup)
	{
		auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
		if (!local_player) return;

		auto animstate = local_player->GetAnimState();
		if (!animstate) return;

		if (SETTINGS::settings.thirdperson_int != -1 && UTILS::INPUT::input_handler.GetKeyState(SETTINGS::settings.thirdperson_int) & 1)
			in_tp = !in_tp;

		if (INTERFACES::Engine->IsConnected() && INTERFACES::Engine->IsInGame())
		{
			GrenadePrediction::instance().View(pSetup);
			auto GetCorrectDistance = [&local_player](float ideal_distance) -> float //lambda
			{
				Vector inverse_angles;
				INTERFACES::Engine->GetViewAngles(inverse_angles);

				inverse_angles.x *= -1.f, inverse_angles.y += 180.f;

				Vector direction;
				MATH::AngleVectors(inverse_angles, &direction);

				SDK::CTraceWorldOnly filter;
				SDK::trace_t trace;
				SDK::Ray_t ray;

				ray.Init(local_player->GetVecOrigin() + local_player->GetViewOffset(), (local_player->GetVecOrigin() + local_player->GetViewOffset()) + (direction * (ideal_distance + 5.f)));
				INTERFACES::Trace->TraceRay(ray, MASK_ALL, &filter, &trace);

				return ideal_distance * trace.flFraction;
			};

			if (SETTINGS::settings.tp_bool && in_tp)
			{
				if (local_player->GetHealth() <= 0)
					local_player->SetObserverMode(5);

				if (!INTERFACES::Input->m_fCameraInThirdPerson)
				{
					INTERFACES::Input->m_fCameraInThirdPerson = true;
					INTERFACES::Input->m_vecCameraOffset = Vector(GLOBAL::real_angles.x, GLOBAL::real_angles.y, GetCorrectDistance(100));

					Vector camForward;
					MATH::AngleVectors(Vector(INTERFACES::Input->m_vecCameraOffset.x, INTERFACES::Input->m_vecCameraOffset.y, 0), &camForward);
				}
			}
			else
			{
				INTERFACES::Input->m_fCameraInThirdPerson = false;
				INTERFACES::Input->m_vecCameraOffset = Vector(GLOBAL::real_angles.x, GLOBAL::real_angles.y, 0);
			}
			auto zoomsensration = INTERFACES::cvar->FindVar("zoom_sensitivity_ratio_mouse");
			if (SETTINGS::settings.fixscopesens)
				zoomsensration->SetValue("0");
			else
				zoomsensration->SetValue("1");

			if (SETTINGS::settings.aim_type == 0)
			{
				if (!local_player->GetIsScoped())
					pSetup->fov = SETTINGS::settings.fov_val;
				else if (local_player->GetIsScoped() && SETTINGS::settings.removescoping)
					pSetup->fov = SETTINGS::settings.fov_val;
			}
			else if (!(SETTINGS::settings.aim_type == 0) && !local_player->GetIsScoped())
				pSetup->fov = 90;
		}
		original_override_view(ecx, pSetup);
	}
	void __fastcall HookedTraceRay(void *thisptr, void*, const SDK::Ray_t &ray, unsigned int fMask, SDK::ITraceFilter *pTraceFilter, SDK::trace_t *pTrace)
	{
		original_trace_ray(thisptr, ray, fMask, pTraceFilter, pTrace);
		if (INTERFACES::Engine->IsConnected() && INTERFACES::Engine->IsInGame())
			pTrace->surface.flags |= SURF_SKY;
	}
	bool __fastcall HookedGetBool(void* pConVar, void* edx)
	{
		if ((uintptr_t)_ReturnAddress() == CAM_THINK)
			return true;

		return original_get_bool(pConVar);
	}
	float __fastcall GetViewmodelFOV()
	{
		if (INTERFACES::Engine->IsConnected() && INTERFACES::Engine->IsInGame())
		{
			float player_fov = original_viewmodel_fov();

			if (SETTINGS::settings.esp_bool)
				player_fov = SETTINGS::settings.viewfov_val;

			return player_fov;
		}
	}
	void InitHooks()
	{
		iclient_hook_manager.Init(INTERFACES::Client);
		original_frame_stage_notify = reinterpret_cast<FrameStageNotifyFn>(iclient_hook_manager.HookFunction<FrameStageNotifyFn>(37, HookedFrameStageNotify));

		panel_hook_manager.Init(INTERFACES::Panel);
		original_paint_traverse = reinterpret_cast<PaintTraverseFn>(panel_hook_manager.HookFunction<PaintTraverseFn>(41, HookedPaintTraverse));

		model_render_hook_manager.Init(INTERFACES::ModelRender);
		original_draw_model_execute = reinterpret_cast<DrawModelExecuteFn>(model_render_hook_manager.HookFunction<DrawModelExecuteFn>(21, HookedDrawModelExecute));

		scene_end_hook_manager.Init(INTERFACES::RenderView);
		original_scene_end = reinterpret_cast<SceneEndFn>(scene_end_hook_manager.HookFunction<SceneEndFn>(9, HookedSceneEnd));

		trace_hook_manager.Init(INTERFACES::Trace);
		original_trace_ray = reinterpret_cast<TraceRayFn>(trace_hook_manager.HookFunction<TraceRayFn>(5, HookedTraceRay));

		override_view_hook_manager.Init(INTERFACES::ClientMode);
		original_override_view = reinterpret_cast<OverrideViewFn>(override_view_hook_manager.HookFunction<OverrideViewFn>(18, HookedOverrideView));
		original_create_move = reinterpret_cast<CreateMoveFn>(override_view_hook_manager.HookFunction<CreateMoveFn>(24, HookedCreateMove));
		original_viewmodel_fov = reinterpret_cast<GetViewmodelFOVFn>(override_view_hook_manager.HookFunction<GetViewmodelFOVFn>(35, GetViewmodelFOV));
		INTERFACES::cvar->get_convar("viewmodel_offset_x")->callbacks.SetSize(0);
		INTERFACES::cvar->get_convar("viewmodel_offset_y")->callbacks.SetSize(0);
		INTERFACES::cvar->get_convar("viewmodel_offset_z")->callbacks.SetSize(0);
		auto sv_cheats = INTERFACES::cvar->FindVar("sv_cheats");
		get_bool_manager = VMT::VMTHookManager(reinterpret_cast<DWORD**>(sv_cheats));
		original_get_bool = reinterpret_cast<SvCheatsGetBoolFn>(get_bool_manager.HookFunction<SvCheatsGetBoolFn>(13, HookedGetBool));
	}
	void EyeAnglesPitchHook(const SDK::CRecvProxyData *pData, void *pStruct, void *pOut)
	{
		*reinterpret_cast<float*>(pOut) = pData->m_Value.m_Float;

		auto entity = reinterpret_cast<SDK::CBaseEntity*>(pStruct);
		if (!entity)
			return;

	}
	void EyeAnglesYawHook(const SDK::CRecvProxyData *pData, void *pStruct, void *pOut)
	{
		*reinterpret_cast<float*>(pOut) = pData->m_Value.m_Float;

		auto entity = reinterpret_cast<SDK::CBaseEntity*>(pStruct);
		if (!entity)
			return;
	}
	void InitNetvarHooks()
	{
		UTILS::netvar_hook_manager.Hook("DT_CSPlayer", "m_angEyeAngles[0]", EyeAnglesPitchHook);
		UTILS::netvar_hook_manager.Hook("DT_CSPlayer", "m_angEyeAngles[1]", EyeAnglesYawHook);
	}
}