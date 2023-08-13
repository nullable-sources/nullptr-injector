#pragma once
#include "core/ui/model-view-controller/data-model.h"

namespace core::ui::mvc {
	class c_dll_list : public i_data_model {
	private:
		struct dll_t {
		public:
			std::string name{ }, path{ }, architecture{ };

		public:
			dll_t() { }
			dll_t(std::string_view _path);

		public:
			bool selected();
			void select(bool new_select);
		}; static inline std::vector<dll_t> dlls{ };

		static inline std::string selected_dll_path{ "" };
		static inline bool adding_locked{ };

		static inline std::condition_variable data_sync_gate{ }, resume_gate{ };
		static inline std::thread file_selecting_worker{ };
		static inline std::mutex sync_lock{ };

	public:
		c_dll_list() {
			event_dispatcher.attach_listener(e_event_type::create, this);
			event_dispatcher.attach_listener(e_event_type::late_update, this);
		}

		~c_dll_list() {
			file_selecting_worker.detach();

			event_dispatcher.detach_listener(e_event_type::create, this);
			event_dispatcher.detach_listener(e_event_type::late_update, this);
		}

	public:
		static void file_selecting();

		void add_new(Rml::DataModelHandle model_handle, Rml::Event& event, const Rml::VariantList& arguments);
		void remove_selected(Rml::DataModelHandle model_handle, Rml::Event& event, const Rml::VariantList& arguments);
		void inject_selected(Rml::DataModelHandle model_handle, Rml::Event& event, const Rml::VariantList& arguments);

		static int drag_and_drop(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	public:
		void create() override;
		void late_update() override;
	} dll_list{ };
}
