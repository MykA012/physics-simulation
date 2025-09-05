#pragma once
#include "imgui.h"
#include "general.h"

void drawTabBar();


void drawUI()
{
	if (ImGui::Begin("Menu"))
	{
		drawTabBar();

		ImGui::End();
	}
}


void drawTabBar()
{
	if (ImGui::BeginTabBar("##"))
	{
		// Objects Tab
		if (ImGui::BeginTabItem("Objects"))
		{
			// Objects
			for (int i = 0; i < objects.size(); i++)
			{
				auto object = objects[i];
				ImGui::PushID(i);

				ImGui::SeparatorText("Object");

				// Color
				float color[3] = { object->color.r / 255.0f, object->color.g / 255.0f, object->color.b / 255.0f };
				ImGui::Text("Color");
				ImGui::ColorEdit3("##color", color);
				object->color.r = static_cast<int>(color[0] * 255.0f);
				object->color.g = static_cast<int>(color[1] * 255.0f);
				object->color.b = static_cast<int>(color[2] * 255.0f);

				ImGui::Separator();

				// Object Parameters
				ImGui::Text("Object Parameters");
				ImGui::DragFloat("Resolution", &object->res, 10.0f, 100.0f, 100000.0f, "%.0f");
				ImGui::DragFloat("Radius", &object->radius, 1.0f, 1.0f, 200.0f);
				ImGui::DragFloat("Mass", &object->mass, 0.1f, 1.0f, 1000.0f);

				ImGui::Separator();

				// Coordinates
				ImGui::Text("Coordinates");
				ImGui::DragFloat2("Position", object->position.data());
				ImGui::DragFloat2("Velocity", object->velocity.data());
				ImGui::Spacing();

				ImGui::PopID();
			}

			// Buttons
			if (ImGui::Button("Reset"))
			{
				for (int i = 0; i < objects.size(); i++)
				{
					objects[i]->mass = initialObjects[i].mass;
					objects[i]->position = initialObjects[i].position;
					objects[i]->velocity = initialObjects[i].velocity;
					objects[i]->radius = initialObjects[i].radius;
					objects[i]->color = initialObjects[i].color;
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Pause"))
			{
				isPaused = !isPaused;
			}

			ImGui::EndTabItem();
		}

		// Background Tab
		if (ImGui::BeginTabItem("Background"))
		{
			ImGui::SeparatorText("BackGround");
			float color[3] = { bgColor.r / 255.0f, bgColor.g / 255.0f, bgColor.b / 255.0f };
			ImGui::ColorEdit3("##color", color);
			bgColor.r = static_cast<int>(color[0] * 255.0f);
			bgColor.g = static_cast<int>(color[1] * 255.0f);
			bgColor.b = static_cast<int>(color[2] * 255.0f);

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
}
