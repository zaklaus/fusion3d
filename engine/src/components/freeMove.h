/*
 * Copyright (C) 2015 Subvision Studio
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef FREEMOVE_H
#define FREEMOVE_H "Freemove"

#include "../core/math3d.h"
#include "../core/entityComponent.h"

class FreeMove : public EntityComponent
{
public:
	FCLASS (FreeMove);
	FreeMove() {}
	FreeMove(float speed, int forwardKey = Input::KEY_W, int backKey = Input::KEY_S, int leftKey = Input::KEY_A, int rightKey = Input::KEY_D, int shiftKey = Input::KEY_LSHIFT)  :
		m_speed(speed),
		m_forwardKey(forwardKey),
		m_backKey(backKey),
		m_leftKey(leftKey),
		m_rightKey(rightKey),
		m_shiftKey(shiftKey) {}
	
	virtual void ProcessInput(const Input& input, float delta);

	virtual void DebugDrawUI()
	{
		ImGui::InputFloat("Speed", &m_speed);
	}

protected:
private:
	void Move(const Vector3f& direction, float amt);

	float m_speed;
	int m_forwardKey;
	int m_backKey;
	int m_leftKey;
	int m_rightKey;
	int m_shiftKey;
};

#endif // FREEMOVE_H
