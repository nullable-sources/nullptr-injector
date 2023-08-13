#pragma once
#include "core/ui/model-view-controller/data-model.h"

namespace core::ui::mvc {
	class c_process_list : public i_data_model {
	private:
		struct process_t {
		public:
			std::string name{ }, type{ }, architecture{ };
			size_t pid{ };

			bool unique_name{ };

		public:
			bool can_show();

			bool selected();
			void select(bool new_select);
		}; static inline std::vector<process_t> processes{ }; static inline size_t processes_count{ };

		static inline std::string name_filter{ };
		static inline std::vector<std::string> filters{
		#ifdef _WIN64
			"x64"
		#else
			"x86"
		#endif
		};

		static inline std::string selected_process_name{ };
		static inline int selected_pid{ };

		static inline std::condition_variable data_sync_gate{ };
		static inline std::thread processes_updating_worker{ };
		static inline std::mutex sync_lock{ };

	public:
		c_process_list() {
			event_dispatcher.attach_listener(e_event_type::create, this);
			event_dispatcher.attach_listener(e_event_type::late_update, this);
		}

		~c_process_list() {
			processes_updating_worker.detach();

			event_dispatcher.detach_listener(e_event_type::create, this);
			event_dispatcher.detach_listener(e_event_type::late_update, this);
		}

	public:
		static void processes_updating();

	public:
		void create() override;
		void late_update() override;
	} inline process_list{ };
}