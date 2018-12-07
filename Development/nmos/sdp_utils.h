#ifndef NMOS_SDP_UTILS_H
#define NMOS_SDP_UTILS_H

#include "cpprest/basic_utils.h"
#include "sdp/json.h"
#include "sdp/ntp.h"
#include "nmos/rational.h"

namespace nmos
{
    struct sdp_parameters;

    // Sender helper functions

    sdp_parameters make_sdp_parameters(const web::json::value& source, const web::json::value& flow, const web::json::value& sender, const std::vector<utility::string_t>& media_stream_ids);

    web::json::value make_session_description(const sdp_parameters& sdp_params, const web::json::value& transport_params);

    // Receiver helper functions

    // Get transport parameters from the parsed SDP file
    web::json::value get_session_description_transport_params(const web::json::value& session_description);

    // Get other SDP parameters from the parsed SDP file
    sdp_parameters get_session_description_sdp_parameters(const web::json::value& session_description);

    std::pair<sdp_parameters, web::json::value> parse_session_description(const web::json::value& session_description);

    void validate_sdp_parameters(const web::json::value& receiver, const sdp_parameters& sdp_params);

    struct sdp_parameters
    {
        struct origin_t
        {
            utility::string_t user_name;
            uint64_t session_id;
            uint64_t session_version;

            origin_t() : session_id(), session_version() {}
            origin_t(const utility::string_t& user_name, uint64_t session_id, uint64_t session_version)
                : user_name(user_name)
                , session_id(session_id)
                , session_version(session_version)
            {}
            origin_t(const utility::string_t& user_name, uint64_t session_id_version)
                : user_name(user_name)
                , session_id(session_id_version)
                , session_version(session_id_version)
            {}
        } origin;

        utility::string_t session_name;

        struct connection_data_t
        {
            uint32_t ttl;

            connection_data_t() : ttl() {}
            connection_data_t(uint32_t ttl) : ttl(ttl) {}
        } connection_data;

        struct timing_t
        {
            uint64_t start_time;
            uint64_t stop_time;
            timing_t() : start_time(), stop_time() {}
            timing_t(uint64_t start_time, uint64_t stop_time) : start_time(start_time), stop_time(stop_time) {}
        } timing;

        struct group_t
        {
            sdp::group_semantics_type semantics;
            // stream identifiers for each leg when redundancy is being used, in the appropriate order
            std::vector<utility::string_t> media_stream_ids;

            group_t() {}
            group_t(const sdp::group_semantics_type& semantics, const std::vector<utility::string_t>& media_stream_ids) : semantics(semantics), media_stream_ids(media_stream_ids) {}
        } group;

        sdp::media_type media_type;
        sdp::protocol protocol;

        struct rtpmap_t
        {
            uint64_t payload_type;
            // encoding-name is "raw" for video, "L24" or "L16" for audio, "smpte291" for data
            utility::string_t encoding_name;
            uint64_t clock_rate;

            rtpmap_t() : payload_type(), clock_rate() {}
            rtpmap_t(uint64_t payload_type, const utility::string_t& encoding_name, uint64_t clock_rate)
                : payload_type(payload_type)
                , encoding_name(encoding_name)
                , clock_rate(clock_rate)
            {}
        } rtpmap;

        // additional "video/raw" parameters (video only)
        struct video_t
        {
            // fmtp indicates format
            uint32_t width;
            uint32_t height;
            nmos::rational exactframerate;
            bool interlace;
            sdp::sampling sampling;
            uint32_t depth;
            sdp::transfer_characteristic_system tcs; // nmos::transfer_characteristic is a subset
            sdp::colorimetry colorimetry; // nmos::colorspace is a subset
            sdp::type_parameter tp;

            video_t() : width(), height(), depth() {}
            video_t(uint32_t width, uint32_t height, const nmos::rational& exactframerate, bool interlace, const sdp::sampling& sampling, uint32_t depth, const sdp::transfer_characteristic_system& tcs, const sdp::colorimetry& colorimetry, const sdp::type_parameter& tp)
                : width(width)
                , height(height)
                , exactframerate(exactframerate)
                , interlace(interlace)
                , sampling(sampling)
                , depth(depth)
                , tcs(tcs)
                , colorimetry(colorimetry)
                , tp(tp)
            {}
        } video;

        // additional "audio/L" parameters (audio only)
        struct audio_t
        {
            // rtpmap encoding-parameters indicates channel_count
            uint32_t channel_count;
            // rtpmap encoding-name (e.g. "L24") indicates bit_depth
            uint32_t bit_depth;
            // rtpmap clock-rate indicates sample_rate
            nmos::rational sample_rate;

            // fmtp indicates channel-order (e.g. "SMPTE2110.(ST)")
            utility::string_t channel_order;

            // ptime
            double packet_time;

            audio_t() : channel_count(), bit_depth(), packet_time() {}
            audio_t(uint32_t channel_count, uint32_t bit_depth, const nmos::rational& sample_rate, const utility::string_t& channel_order, double packet_time)
                : channel_count(channel_count)
                , bit_depth(bit_depth)
                , sample_rate(sample_rate)
                , channel_order(channel_order)
                , packet_time(packet_time)
            {}
        } audio;

        // additional "video/smpte291" data parameters (data only)
        struct data_t
        {
        } data;

        struct ts_refclk_t
        {
            // support ptp, and localmac?
            sdp::ts_refclk_source clock_source;
            sdp::ptp_version ptp_version;
            utility::string_t ptp_server;

            ts_refclk_t() {}
            ts_refclk_t(const sdp::ts_refclk_source& clock_source, const sdp::ptp_version& ptp_version, const utility::string_t& ptp_server)
                : clock_source(clock_source)
                , ptp_version(ptp_version)
                , ptp_server(ptp_server)
            {}
        } ts_refclk;

        struct mediaclk_t
        {
            sdp::mediaclk_source clock_source;
            utility::string_t clock_parameters;

            mediaclk_t() {}
            mediaclk_t(const sdp::mediaclk_source& clock_source, const utility::string_t& clock_parameters)
                : clock_source(clock_source)
                , clock_parameters(clock_parameters)
            {}
        } mediaclk;

        // construct null SDP parameters
        sdp_parameters() {}

        // construct "video/raw" SDP parameters with sensible defaults for unspecified fields
        sdp_parameters(const utility::string_t& session_name, const video_t& video, uint64_t payload_type, const std::vector<utility::string_t>& media_stream_ids = {})
            : origin(U("-"), sdp::ntp_now() >> 32)
            , session_name(session_name)
            , connection_data(32)
            , timing()
            , group(!media_stream_ids.empty() ? group_t{ sdp::group_semantics::duplication, media_stream_ids } : group_t{})
            , media_type(sdp::media_types::video)
            , protocol(sdp::protocols::RTP_AVP)
            , rtpmap(payload_type, U("raw"), 90000)
            , video(video)
            , audio()
            , data()
            , ts_refclk(sdp::ts_refclk_sources::ptp, sdp::ptp_versions::IEEE1588_2008, U("traceable"))
            , mediaclk(sdp::mediaclk_sources::direct, U("0"))
        {}

        // construct "audio/L" SDP parameters with sensible defaults for unspecified fields
        sdp_parameters(const utility::string_t& session_name, const audio_t& audio, uint64_t payload_type, const std::vector<utility::string_t>& media_stream_ids = {})
            : origin(U("-"), sdp::ntp_now() >> 32)
            , session_name(session_name)
            , connection_data(32)
            , timing()
            , group(!media_stream_ids.empty() ? group_t{ sdp::group_semantics::duplication, media_stream_ids } : group_t{})
            , media_type(sdp::media_types::audio)
            , protocol(sdp::protocols::RTP_AVP)
            , rtpmap(payload_type, U("L") + utility::ostringstreamed(audio.bit_depth), uint64_t(double(audio.sample_rate.numerator()) / double(audio.sample_rate.denominator()) + 0.5))
            , video()
            , audio(audio)
            , data()
            , ts_refclk(sdp::ts_refclk_sources::ptp, sdp::ptp_versions::IEEE1588_2008, U("traceable"))
            , mediaclk(sdp::mediaclk_sources::direct, U("0"))
        {}

        // construct "video/smpte291" SDP parameters with sensible defaults for unspecified fields
        sdp_parameters(const utility::string_t& session_name, const data_t& data, uint64_t payload_type, const std::vector<utility::string_t>& media_stream_ids = {})
            : origin(U("-"), sdp::ntp_now() >> 32)
            , session_name(session_name)
            , connection_data(32)
            , timing()
            , group(!media_stream_ids.empty() ? group_t{ sdp::group_semantics::duplication, media_stream_ids } : group_t{})
            , media_type(sdp::media_types::video)
            , protocol(sdp::protocols::RTP_AVP)
            , rtpmap(payload_type, U("smpte291"), 90000)
            , video()
            , audio()
            , data(data)
            , ts_refclk(sdp::ts_refclk_sources::ptp, sdp::ptp_versions::IEEE1588_2008, U("traceable"))
            , mediaclk(sdp::mediaclk_sources::direct, U("0"))
        {}
    };
}

#endif
