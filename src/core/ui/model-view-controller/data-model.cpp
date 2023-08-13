#include "core/ui/ui.h"
#include "data-model.h"

namespace core::ui::mvc {
	void initialize() {
		constructor = context->CreateDataModel("data_model");
		if(!constructor) {
			utils::logger(utils::e_log_type::error, "Cant create data model constructor");
			return;
		}

		event_dispatcher.create();

		handle = constructor.GetModelHandle();
	}
}