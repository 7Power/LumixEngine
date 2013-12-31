#pragma once


#include "core/vector.h"
#include "gui/block.h"


class MainFrame;


class Notifications : public Lux::UI::Block
{
	public:
		Notifications(MainFrame& main_frame);
		void update();
		void showNotification(const char* text);

	private:
		struct Notification
		{
			Lux::UI::Block* m_ui;
			uint32_t m_time;
		};

		Lux::vector<Notification> m_notifications;
};