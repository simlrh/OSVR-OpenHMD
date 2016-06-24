// Internal Includes
#include <osvr/PluginKit/PluginKit.h>
#include <osvr/PluginKit/TrackerInterfaceC.h>

// Generated JSON header file
#include "je_nourish_openhmd_json.h"

// Library/third-party includes
#include <openhmd.h>

// Standard includes
#include <stdlib.h>
#include <iostream>
#include <string>
#include <list>

namespace {

	class OculusHMD {
		public:
			OculusHMD(OSVR_PluginRegContext ctx, ohmd_context* ohmd_ctx, int i) : m_context(ohmd_ctx) {
				std::string name ("Oculus ");
				name.append(ohmd_list_gets(m_context, i, OHMD_PRODUCT));

				m_device = ohmd_list_open_device(m_context, i);

				OSVR_DeviceInitOptions opts = osvrDeviceCreateInitOptions(ctx);
				osvrDeviceTrackerConfigure(opts, &m_tracker);

				m_dev.initAsync(ctx, name.c_str(), opts);
				m_dev.sendJsonDescriptor(je_nourish_openhmd_json);
				m_dev.registerUpdateCallback(this);

				osvrQuatSetIdentity(&m_state);
			}

			~OculusHMD() {
				ohmd_close_device(m_device);
			}

			OSVR_ReturnCode update() {
				ohmd_ctx_update(m_context);

				float quat[4];
				ohmd_device_getf(m_device, OHMD_ROTATION_QUAT, quat);

				osvrQuatSetX(&m_state, quat[0]);
				osvrQuatSetY(&m_state, quat[1]);
				osvrQuatSetZ(&m_state, quat[2]);
				osvrQuatSetW(&m_state, quat[3]);

				osvrDeviceTrackerSendOrientation(m_dev, m_tracker, &m_state, 0);

				return OSVR_RETURN_SUCCESS;
			}

		private:
			osvr::pluginkit::DeviceToken m_dev;
			OSVR_TrackerDeviceInterface m_tracker;
			OSVR_Quaternion m_state;
			ohmd_context* m_context;
			ohmd_device* m_device;
	};

	class HardwareDetection {
		public:
			HardwareDetection() : m_found(false) {
				ohmd_ctx = ohmd_ctx_create();
			}

			~HardwareDetection() {
				ohmd_ctx_destroy(ohmd_ctx);
			}

			OSVR_ReturnCode operator()(OSVR_PluginRegContext ctx) {

                if (!m_found) {
                    int num_devices = ohmd_ctx_probe(ohmd_ctx);

                    if (num_devices < 0) {
                        std::cout << "OpenHMD failed to probe devices: " << ohmd_ctx_get_error(ohmd_ctx) << std::endl;
                        return OSVR_RETURN_FAILURE;
                    }

                    std::string path;
                    std::string product;

                    for (int i = 0; i < num_devices; i++) {

                        product = ohmd_list_gets(ohmd_ctx, i, OHMD_PRODUCT);

                        if (product.compare("Rift (Devkit)") == 0) {
                            osvr::pluginkit::registerObjectForDeletion(
                                    ctx, new OculusHMD(ctx, ohmd_ctx, i));
                            m_found = true;
                        }
                    }
                }


				return OSVR_RETURN_SUCCESS;
			}

		private:
			ohmd_context* ohmd_ctx;
			bool m_found;


	};
} // namespace

OSVR_PLUGIN(je_nourish_openhmd) {
	osvr::pluginkit::PluginContext context(ctx);

	context.registerHardwareDetectCallback(new HardwareDetection());

	return OSVR_RETURN_SUCCESS;
}
