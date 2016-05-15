#include "devMode.h"

bool general_settings = false;
Entity* general_settings_uid = 0;
DevMode* root_entity = 0;
std::vector<const char*> factory_class_names;

bool create_entity_window = false;
bool create_component_window = false;

bool component_settings = false;
EntityComponent* component_settings_uid = 0;

void CreateEntity()
{
	static char name[256] = { 0 };
	//Dialog
	{
		ImGui::InputText("Name", name, 256);
		
	}

	if (ImGui::Button("Cancel"))
	{
clear:
		ZeroMemory(name, 256);
		create_entity_window = false;
	}

	if (ImGui::Button("Create")) 
	{
		auto e = new Entity();
		e->SetDisplayName(name);
		e->SetTransform(*root_entity->GetTransform());

		root_entity->GetParent()->GetScene()->AddChild(e);
		general_settings = true;
		general_settings_uid = e;

		goto clear;
	}
}

void CreateComponent()
{
	static int selected_item = 0;
	ImGui::ListBoxHeader("Components");
	{
		for (size_t i = 0; i < factory_class_names.size(); i++)
		{
			ImGui::ListBox(factory_class_names[i], &selected_item, &factory_class_names[0], factory_class_names.size());
		}
	}
	ImGui::ListBoxFooter();

	if (ImGui::Button("Create Component"))
	{
		general_settings_uid->AddComponent((EntityComponent*) g_factory.construct(factory_class_names[selected_item]));
		selected_item = 0;
		create_component_window = false;
	}
}

void ShowCompProps(EntityComponent* uid)
{
	ImGui::Text("Component: %s", uid->__ClassType__().c_str());

	ImGui::Separator();

	uid->DrawDebugUI();
}

void ShowObjectProps(Entity* uid)
{
	if (!uid)
		return;

	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);
	{
		ImGui::PushItemWidth(-1);
		{
			if (ImGui::TreeNode("Transform"))
			{
				ImGui::Columns(3);
				auto t = uid->GetTransform();

				Vector3f* p = t->GetPos();
				float x = p->GetX();
				float y = p->GetY();
				float z = p->GetZ();

				ImGui::DragFloat("X ##xp", &x);  
				ImGui::DragFloat("Y ##yp", &y);
				ImGui::DragFloat("Z ##zp", &z);
				t->SetPos(Vector3f(x, y, z));

				ImGui::NextColumn();

				auto p2 = t->GetRot();
				static float x2 = p2->GetX();
				static float y2 = p2->GetY();
				static float z2 = p2->GetZ();
				static float w2 = ToDegrees(p2->GetW());

				ImGui::SliderFloat("X ##xr", &x2, 0, 1);
				ImGui::SliderFloat("Y ##yr", &y2, 0, 1);
				ImGui::SliderFloat("Z ##zr", &z2, 0, 1);
				ImGui::SliderFloat("W ##wr", &w2, -180, 180);

				ImGui::NextColumn();

				t->SetRot(Quaternion(Vector3f(x2,y2,z2),ToRadians(w2)));
				auto p3 = t->GetScale();
				static float x3 = p3;

				ImGui::DragFloat("XYZ ##xs", &x3);

				t->SetScale(x3);

				ImGui::TreePop();
				ImGui::Columns(1);
			}
		}
		ImGui::PopItemWidth();
	}
	ImGui::PopItemWidth();
	ImGui::NextColumn();

	ImGui::Separator();
	{
		if (ImGui::Button("Go To"))
		{
			Vector3f dir = uid->GetTransform()->GetPos() - root_entity->GetTransform()->GetPos();

			Vector3f newPos = *uid->GetTransform()->GetPos() + (dir.Normalized() * -5.0f);

			//root_entity->GetTransform()->LookAt((*uid->GetTransform()->GetPos()), Vector3f(0, 1, 0));
			root_entity->GetTransform()->SetPos(newPos);
		}

		if (ImGui::Button("Teleport To"))
		{
			//root_entity->GetTransform()->LookAt((*uid->GetTransform()->GetPos()), Vector3f(0, 1, 0));
			uid->GetTransform()->SetPos(*root_entity->GetTransform()->GetPos());
		}
	}
	ImGui::Separator();

	if (ImGui::TreeNode("Components"))
	{
		for (int i = 0; i < uid->GetAllComponents().size(); i++)
		{
			if (uid->GetAllComponents()[i] == nullptr)
				continue;

			ImGui::AlignFirstTextHeightToWidgets();
			// Here we use a Selectable (instead of Text) to highlight on hover
			//ImGui::Text("Field_%d", i);
			char label[64];
			sprintf(label, "Component %s", uid->GetAllComponents()[i]->__ClassType__().c_str());
			ImGui::Bullet();
			if (ImGui::Selectable(label))
			{
				component_settings_uid = uid->GetAllComponents()[i];
				component_settings = true;
			}
			//ImGui::NextColumn();
			/*ImGui::PushItemWidth(-1);
			ImGui::PopItemWidth();*/
		}
		ImGui::TreePop();
	}

	if (ImGui::Button("Create Component"))
	{
		create_component_window = true;
	}

}

void ShowObject(const char* prefix, Entity* uid)
{
	if (uid == nullptr)
		return;

	ImGui::PushID(uid);                      // Use object uid as identifier. Most commonly you could also use the object pointer as a base ID.
	//ImGui::AlignFirstTextHeightToWidgets();  // Text and Tree nodes are less high than regular widgets, here we add vertical spacing to make the tree lines equal high.
	bool node_open = ImGui::TreeNode("Object", "%s_%u", prefix, uid);

	if (node_open)
	{
		if (ImGui::Selectable("General", &general_settings)) {
			general_settings = true;
			general_settings_uid = uid;
		}
		for (int i = 0; i < uid->GetAllEntities().size(); i++)
		{
			ImGui::PushID(i); // Use field index as identifier.
			ShowObject(uid->GetAllEntities()[i]->GetDisplayName().c_str(), uid->GetAllEntities()[i]);
			ImGui::PopID();
		}
		ImGui::TreePop();
	}
	ImGui::PopID();
}

void DevMode::Init()
{
	m_move->SetParent(GetParent());
	m_look->SetParent(GetParent());
	m_devCamera.SetTransform(GetTransform());
	m_lastCamera = m_engine->GetRenderingEngine()->GetMainCamera();

	if (m_isDev)
	{
		m_lastCamera = m_engine->GetRenderingEngine()->GetMainCamera();
		m_engine->GetRenderingEngine()->SetMainCamera(m_devCamera);
	}
	else
	{
		if (m_lastCamera != NULL)
			m_engine->GetRenderingEngine()->SetMainCamera(*m_lastCamera);
	}

	m_engine->GetPhysicsEngine()->SetSimulation(false);
	ProgramHoster::SetActive(m_isUpdating);


	for (std::map<std::string, constructor_t>::iterator it = g_factory.m_classes.begin(); it != g_factory.m_classes.end(); ++it) {
		factory_class_names.push_back(it->first.c_str());
	}
}

void DevMode::PostRender (const Shader & shader, const RenderingEngine & renderingEngine, const Camera & camera)
{
	root_entity = this;
	if (m_isDev)
	{
		btVector3 color;

		switch (m_handleMode)
		{
		case HANDLE_SELECT:
			color = btVector3 (0, 1, 0);
			break;
		case HANDLE_TRANSFORM:
			color = btVector3 (1, 0, 0);
			break;
		case HANDLE_ROTATE:
			color = btVector3 (1, 1, 0);
			break;
		case HANDLE_SCALE:
			color = btVector3 (0, 0, 1);
			break;
		}

		for (auto x : m_selected)
		{
			//m_engine->GetPhysicsEngine ()->GetWorld ()->debugDrawObject (x->GetBody ()->getWorldTransform (), x->GetBody ()->getCollisionShape (), color);
		}
	}

	/// Interface
	{
		if (general_settings)
		{
			ImGui::SetNextWindowPos(ImVec2(320, 240), ImGuiSetCond_FirstUseEver);
			ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiSetCond_FirstUseEver);
			ImGui::Begin("Property Manager", &general_settings, ImGuiWindowFlags_AlwaysAutoResize);
			{
				ShowObjectProps(general_settings_uid);
				ImGui::End();
			}
		}

		if (create_entity_window)
		{
			ImGui::SetNextWindowPosCenter();
			ImGui::Begin("Create Entity", &create_entity_window, ImGuiWindowFlags_AlwaysAutoResize);
			{
				CreateEntity();
				ImGui::End();
			}
		}
		
		if (create_component_window)
		{
			ImGui::SetNextWindowPosCenter();
			ImGui::Begin("Create Component", &create_component_window, ImGuiWindowFlags_AlwaysAutoResize);
			{
				CreateComponent();
				ImGui::End();
			}
		}

		if (component_settings)
		{
			ImGui::Begin("Component Editor", &component_settings, ImGuiWindowFlags_AlwaysAutoResize);
			{
				ShowCompProps(component_settings_uid);
				ImGui::End();
			}
		}

		ImGui::SetNextWindowPos(ImVec2(320, 240), ImGuiSetCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiSetCond_FirstUseEver);
		ImGui::Begin("Hierarchy Window", &show_entity_manager);
		{
			//ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
			//ImGui::Columns(2);
			ImGui::Separator();
			{
				for (auto x : GetParent()->GetScene()->GetAllEntities())
				{
					ShowObject(x->GetDisplayName().c_str(), x);
				}
			}
			ImGui::Columns(1);
			ImGui::Separator();

			if (ImGui::Button("Create Entity"))
			{
				create_entity_window = true;
			}

			//ImGui::PopStyleVar();
			ImGui::End();
		}

		ImGui::SetNextWindowPos (ImVec2 (10, 80), ImGuiSetCond_FirstUseEver);
		ImGui::Begin ("Stats", (bool*)&m_statsWnd, ImGuiWindowFlags_NoResize);
		{
			ImGui::Text ("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO ().Framerate, ImGui::GetIO ().Framerate);
			ImGui::End();
		}
		

		if (ImGui::BeginMainMenuBar ())
		{
			if (ImGui::BeginMenu ("Game"))
			{
				if (ImGui::MenuItem ("Entity Manager", "CTRL+E"))
				{
					show_entity_manager = !show_entity_manager;
				}

				ImGui::EndMenu ();
			}

			if (ImGui::BeginMenu ("States"))
			{
				if (ImGui::MenuItem ("Enable Physics", "CTRL+P", enable_physics)) 
				{
					enable_physics = !enable_physics;
					m_engine->GetPhysicsEngine()->SetSimulation(enable_physics);
				}
				

				ImGui::EndMenu ();
			}

			ImGui::EndMainMenuBar ();
		}

	}

	///GIZMO
	if (general_settings_uid && m_isDev)
	{
		auto ot = general_settings_uid->GetTransform();
		Shader s = shader;
		//m_gizmoSphere->GetTransform()->SetPos(*ot->GetPos());
		m_gizmoSphere->SetTransform(*ot);
		m_gizmoSphere->RenderAll(s, renderingEngine, camera);

		Vector3f pos = ot->GetTransformedPos();

		pos = (Vector3f) camera.GetViewProjection().Transform(pos);
		pos.SetX(pos.GetX() / pos.GetZ());
		pos.SetY(pos.GetY() / pos.GetZ());

		pos.SetX(renderingEngine.GetWindow()->GetWidth() *  ((pos.GetX() + 1.0f) / 2.0f) + 0.05f);
		pos.SetY(renderingEngine.GetWindow()->GetHeight() * (1.0f - (pos.GetY() + 1.0f) / 2.0f) + 0.05f);

		if (pos.GetX() > renderingEngine.GetWindow()->GetWidth() ||
			pos.GetX() < 0 ||
			pos.GetY() > renderingEngine.GetWindow()->GetHeight() ||
			pos.GetY() < 0)
		{
			return;
		}

		/*ImGui::SetNextWindowPos(ImVec2(pos.GetX(), pos.GetY()));
		ImGui::Begin(general_settings_uid->GetDisplayName().c_str());
		{
			ImGui::End();
		}*/
	}
}