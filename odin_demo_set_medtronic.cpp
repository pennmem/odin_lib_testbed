#include <iostream>
#include <OdinLib.h>
#include <vector>
#include <chrono>
#include <thread>

using namespace std;
int main() {

	int log_level = 3;
	configureDiagnosticOutput(log_level);


	int major_version, minor_version, build_version;
	getSoftwareVersion(&major_version, &minor_version, &build_version);

	cerr << "Major_version=" << major_version << endl;
	cerr << " Minor_version=" << minor_version << endl;
	cerr << "build_version=" << build_version << endl;

	bool autoConnect = true;
	int writingInterval = 10;
	int interval = writingInterval;

	int configureEnsSimulatorOptions_status = configureEnsSimulatorOptions(autoConnect, writingInterval);

	cerr << "configureEnsSimulatorOptions_status=" << configureEnsSimulatorOptions_status << endl;


	bool useSimulator = true, showSimulatorGUI = true, useUsb = false;
	int theTimeOut = 2 * writingInterval;

	int startEnsCommunicationSession_status = startEnsCommunicationSession(useSimulator, showSimulatorGUI, useUsb, theTimeOut);

	cerr << "startEnsCommunicationSession_status=" << startEnsCommunicationSession_status << endl;
	int startSensing_status = startSensing();

	std::this_thread::sleep_for(std::chrono::milliseconds(2 * writingInterval));


	int num_channels = 4;
	//allocate buffers
	std::size_t counter = 0;
	std::size_t tic_counter = 0;
	std::vector<short> data_buffer(100000, 0);
	std::vector<unsigned int> tic_count_buffer(100000, 0);
	std::vector<short> single_sample_set_data_buffer(1000, 0);
	std::vector<unsigned int> tic_count_sample_set_data_buffer(1000, 0);

	int num_samples_available = 0;
	unsigned int num_samples = 0;

	num_samples = num_samples_available; //OK since to get here we need to have some number of samples in the ENS buffer

	unsigned int num_tic = 0;
	unsigned int current_tic_idx = 0;


	for (int i = 0; i < 20; ++i) {
		auto getNumSamplesAvail_status = getNumSamplesAvail(&num_samples_available);
		cerr << "num_samples_available=" << num_samples_available << endl;
		if ((getNumSamplesAvail_status >= 0) && num_samples_available) {
			num_samples = num_samples_available;
			auto getNextSampleSet_status = getNextSampleSet(num_channels, &num_samples, &tic_count_sample_set_data_buffer[0], &single_sample_set_data_buffer[0], 0);
			if (num_samples) {
				std::size_t tics_retrieved = num_samples / num_channels;
				data_buffer.insert(data_buffer.begin() + counter, single_sample_set_data_buffer.begin(), single_sample_set_data_buffer.begin() + num_samples);

				tic_count_buffer.insert(tic_count_buffer.begin() + tic_counter, tic_count_sample_set_data_buffer.begin(), tic_count_sample_set_data_buffer.begin() + tics_retrieved);
				counter += num_samples;
				tic_counter += tics_retrieved;

			}
		}
		else {

			std::this_thread::sleep_for(std::chrono::milliseconds(interval));
		}
	}

	int stopSensing_status = stopSensing();
	int stopEnsCommunicationSession_status = stopEnsCommunicationSession();
	int stopEnsSimulator_status = stopEnsSimulator();


	for (size_t idx = 0; idx < tic_counter; ++idx) {
		cerr << "tic[" << idx << "]=" << tic_count_buffer[idx] << endl;
		for (auto j = 0; j < num_channels; ++j) {
			cerr << "buf[" << j << "]=" << data_buffer[idx*num_channels + j] << " ";
		}
		cerr << endl;
	}

	return 0;



}