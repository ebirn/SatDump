#pragma once

#include "common/dsp_sample_source/dsp_sample_source.h"
#include <rtl-sdr.h>
#include "logger.h"
#include "imgui/imgui.h"
#include "core/style.h"
#include <thread>

class RtlSdrSource : public dsp::DSPSampleSource
{
protected:
    bool is_open = false, is_started = false;
    rtlsdr_dev *rtlsdr_dev_obj;
    static void _rx_callback(unsigned char *buf, uint32_t len, void *ctx);

    int selected_samplerate = 0;
    std::string samplerate_option_str;
    std::vector<uint64_t> available_samplerates;
    uint64_t current_samplerate = 0;

    int gain = 0;
    bool bias_enabled = false;
    bool lna_agc_enabled = false;

    void set_gains();
    void set_bias();
    void set_agcs();

    std::thread work_thread;

    bool thread_should_run = false, needs_to_run = false;

    void mainThread()
    {
        while (thread_should_run)
        {
            if (needs_to_run)
            {
                rtlsdr_read_async(rtlsdr_dev_obj, _rx_callback, &output_stream, 0, 16384);
            }
            else
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
    }

public:
    RtlSdrSource(dsp::SourceDescriptor source) : DSPSampleSource(source)
    {
        thread_should_run = true;
        work_thread = std::thread(&RtlSdrSource::mainThread, this);
    }

    ~RtlSdrSource()
    {
        stop();
        close();

        thread_should_run = false;
        logger->info("Waiting for the thread...");
        if (is_started)
            output_stream->stopWriter();
        if (work_thread.joinable())
            work_thread.join();
        logger->info("Thread stopped");
    }

    void set_settings(nlohmann::json settings);
    nlohmann::json get_settings(nlohmann::json);

    void open();
    void start();
    void stop();
    void close();

    void set_frequency(uint64_t frequency);

    void drawControlUI();

    void set_samplerate(uint64_t samplerate);
    uint64_t get_samplerate();

    static std::string getID() { return "rtlsdr"; }
    static std::shared_ptr<dsp::DSPSampleSource> getInstance(dsp::SourceDescriptor source) { return std::make_shared<RtlSdrSource>(source); }
    static std::vector<dsp::SourceDescriptor> getAvailableSources();
};