#pragma once
#include <Blobs.hpp>
#include <BlobUtils.hpp>

#include "Scene.hpp"

class MainScene : public Scene
{
private:
	enum class State
	{
		INTRO, MENU, HELP, OUTRO, EXIT, NEXT_ROOM
	};

public:
	MainScene(const size_t& id)
		: Scene(id)
		, textureRenderer()
		, titleCoords(0.0f, 0.4f, 0.0f)
		, menuSelected(0), lastMenuIndex(2)
		, myStatus(State::INTRO), doingAppQuit(false)
		, introFadePeriod(0.8f), outroFadePeriod(1.0f), outroTitlePeriod(0.7f)
		, introFadeTime(introFadePeriod), outroFadeTime(outroFadePeriod)
		, outroTitleTime()
	{
		SetName("MainScene");
	}

	void Awake() override;

	void Start() override
	{
		Scene::Start();

		textureRenderer.Start();

		titleCoords.y = 0.4f;

		myStatus = State::INTRO;
		introFadeTime = introFadePeriod;
		outroTitleTime = outroTitlePeriod;
		outroFadeTime = outroFadePeriod;
	}

	void Update() override
	{
		const auto delta_time = Timer::GetDeltaTime();

		switch (myStatus)
		{
			case State::INTRO:
			{
				if (0 < introFadeTime)
				{
					introFadeTime -= delta_time;
				}
				else
				{
					myStatus = State::MENU;
					introFadeTime = 0;
				}
			}
			break;

			case State::MENU:
			{

			}
			break;

			case State::OUTRO:
			{
				if (!doingAppQuit)
				{
					if (0 < outroTitleTime)
					{
						outroTitleTime -= delta_time;
					}
					else
					{
						outroTitleTime = 0;
					}

					if (outroTitleTime < outroTitlePeriod * 0.6f)
					{
						if (0 < outroFadeTime)
						{
							outroFadeTime -= delta_time;
						}
						else
						{
							myStatus = State::NEXT_ROOM;

							outroFadeTime = 0;
						}
					}
				}
				else
				{
					if (0 < outroFadeTime)
					{
						outroFadeTime -= delta_time;
					}
					else
					{
						myStatus = State::EXIT;
					}
				}
			}
			break;

			case State::NEXT_ROOM:
			{
				End();
			}
			break;

			case State::EXIT:
			{
				std::quick_exit(0);
			}
			return;
		}
	}

	void OnKeyboard(const unsigned char& key, const int& x, const int& y) override
	{
		switch (key)
		{
			case 13: // Enter
			{
				if (State::HELP == myStatus)
				{
					SetState(State::MENU);
				}
				else if (State::MENU == myStatus)
				{
					if (0 == menuSelected) // 시작
					{
						SetState(State::OUTRO);
					}
					else if (1 == menuSelected) // 도움말
					{
						SetState(State::HELP);
					}
					else if (2 == menuSelected) // 종료
					{
						SetState(State::OUTRO);
						doingAppQuit = true;
					}
				}
				else
				{

				}
			}
			break;

			case 27: // Escape
			{
				if (State::HELP == myStatus)
				{
					SetState(State::MENU);
				}
				else if (State::MENU == myStatus)
				{
					SetState(State::OUTRO);
					doingAppQuit = true;
				}
			}
			break;
		}
	}

	void OnSpecialKey(const int& key, const int& x, const int& y) override
	{
		switch (key)
		{
			case GLUT_KEY_LEFT:
			{
				if (State::MENU == myStatus) SelectPrevMenu();
			}
			break;

			case GLUT_KEY_RIGHT:
			{
				if (State::MENU == myStatus) SelectNextMenu();
			}
			break;

			case GLUT_KEY_UP:
			{
				if (State::MENU == myStatus) SelectPrevMenu();
			}
			break;

			case GLUT_KEY_DOWN:
			{
				if (State::MENU == myStatus) SelectNextMenu();
			}
			break;

			case GLUT_KEY_F4:
			{
				std::quick_exit(0);
			}
			break;
		}
	}

	void Render() override;

	constexpr void SetState(const State& state)
	{
		myStatus = state;
	}

	constexpr void SelectPrevMenu()
	{
		if (0 < menuSelected)
		{
			menuSelected--;
		}
		else
		{
			menuSelected = lastMenuIndex;
		}
	}

	constexpr void SelectNextMenu()
	{
		if (menuSelected < lastMenuIndex)
		{
			menuSelected++;
		}
		else
		{
			menuSelected = 0;
		}
	}

	ogl::Pipeline textureRenderer;

	glm::vec3 titleCoords;

	int menuSelected;
	const int lastMenuIndex;

	State myStatus;
	bool doingAppQuit;
	float introFadeTime;
	const float introFadePeriod;
	float outroTitleTime;
	const float outroTitlePeriod;
	float outroFadeTime;
	const float outroFadePeriod;
};
