#include <iostream>
#include <OdinLib.h>
#include <vector>
#include <chrono>
#include <thread>

using namespace std;
int main(){


	int log_level = 3;
	configureDiagnosticOutput(log_level);


	int major_version, minor_version, build_version;
	getSoftwareVersion(&major_version, &minor_version, &build_version);

	cerr << "Major_version=" << major_version << endl;
	cerr << " Minor_version=" << minor_version << endl;
	cerr << "build_version=" << build_version << endl;

	bool autoConnect=true;
	int writingInterval = 10;
	int interval = writingInterval;

	int configureEnsSimulatorOptions_status = configureEnsSimulatorOptions(autoConnect, writingInterval);

	cerr << "configureEnsSimulatorOptions_status=" << configureEnsSimulatorOptions_status << endl;


	bool useSimulator = true, showSimulatorGUI = true, useUsb = false;
	int theTimeOut = 2 * writingInterval;
		
	int startEnsCommunicationSession_status = startEnsCommunicationSession(useSimulator, showSimulatorGUI, useUsb, theTimeOut);

	cerr << "startEnsCommunicationSession_status=" << startEnsCommunicationSession_status << endl;
	int startSensing_status = startSensing();
	
	std::this_thread::sleep_for(std::chrono::milliseconds(2*writingInterval));


	int num_channels = 4;
	vector<short> buffer_tmp(num_channels,0);

	std::size_t counter = 0;
	std::size_t tic_counter = 0;
	std::vector<short> data_buffer(100000, 0);
	std::vector<unsigned int> tic_count_buffer(100000, 0);

	
	unsigned int sample_tic_count = 0;

	for (unsigned int i = 0 ; i < 100; ++i) {
		int getNextSample_status = getNextSample(num_channels,& sample_tic_count, & buffer_tmp[0]);


		if (getNextSample_status == 1) {
			//cerr << "GOT SAMPLE tic_count="<< sample_tic_count << " "<< buffer_tmp[0] << " , " << buffer_tmp[1] << " , " << buffer_tmp[2] << " , " << buffer_tmp[3] << endl;
			data_buffer.insert(data_buffer.begin() + counter, buffer_tmp.begin(), buffer_tmp.end());
			tic_count_buffer[tic_counter]=sample_tic_count;
			counter += num_channels;
			tic_counter += 1;

		}
		else if (getNextSample_status == 0) {			
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