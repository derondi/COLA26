#include "Config.hh"

Config::Config(std::string &filepath)
{
    XMLDocument doc;
    doc.LoadFile(filepath.c_str());
    XMLElement *element;
    // std::cout << "filepath = " << filepath << std::endl;

    for (element = doc.FirstChildElement("setting")->FirstChildElement("attribute");
         element != NULL;
         element = element->NextSiblingElement("attribute"))
    {
        XMLElement *ele = element->FirstChildElement("name");
        std::string attName = ele->GetText();
        // std::cout << "attName = " << attName << std::endl;
        if (attName == "gc.ldpc.encoder")
        {
            _gc_ldpc_encoder_name = ele->NextSiblingElement("value")->GetText();
        }
        if (attName == "gc.ldpc.encoder.param")
        {
            _gc_ldpc_encoder_param = ele->NextSiblingElement("value")->GetText();
        }
         if (attName == "gc.ldpc.decoder")
        {
            _gc_ldpc_decoder_name = ele->NextSiblingElement("value")->GetText();
        }
        if (attName == "gc.ldpc.decoder.param")
        {
            _gc_ldpc_decoder_param = ele->NextSiblingElement("value")->GetText();
        }
        if (attName == "gc.failure.model")
        {
            _gc_failure_model_name = ele->NextSiblingElement("value")->GetText();
        }
        if (attName == "gc.failure.model.param")
        {
            _gc_failure_model_param = ele->NextSiblingElement("value")->GetText();
        }
        if (attName == "information.bits")
        {
            _information_bits = std::stoll(ele->NextSiblingElement("value")->GetText());
        }
        else if (attName == "parity.bits")
        {
            _parity_bits = std::stoll(ele->NextSiblingElement("value")->GetText());
        }
        else if (attName == "ldpc.encoder")
        {
            _ldpc_encoder_name = ele->NextSiblingElement("value")->GetText();
        }
        else if (attName == "ldpc.encoder.param")
        {
            _ldpc_encoder_param = ele->NextSiblingElement("value")->GetText();
        }
        else if (attName == "failure.model")
        {
            _failure_model_name = ele->NextSiblingElement("value")->GetText();
        }
        else if (attName == "failure.model.param")
        {
            _failure_model_param = ele->NextSiblingElement("value")->GetText();
        }
        else if (attName == "ldpc.decoder")
        {
            _ldpc_decoder_name = ele->NextSiblingElement("value")->GetText();
        }
        else if (attName == "ldpc.decoder.param")
        {
            _ldpc_decoder_param = ele->NextSiblingElement("value")->GetText();
        }
        else if (attName == "rber.rate")
        {
            std::string rberstr = ele->NextSiblingElement("value")->GetText();
            _rber_rate = std::atof(rberstr.c_str());
        }
        else if (attName == "iteration.number")
        {
            _iteration = std::stoll(ele->NextSiblingElement("value")->GetText());
        }
        else if (attName == "msa.alpha")
        {
            _alpha = std::stod(ele->NextSiblingElement("value")->GetText());
        }
        else if (attName == "msa.beta")
        {
            _beta = std::stod(ele->NextSiblingElement("value")->GetText());
        }
        else if (attName == "ssd.type")
        {
            _ssd_type = ele->NextSiblingElement("value")->GetText();
        }
        else if (attName == "ssd.capacity.GB")
        {
            _ssd_capacity_GB = std::stoll(ele->NextSiblingElement("value")->GetText());
        }
        else if (attName == "ssd.page.KB")
        {
            _ssd_page_KB = std::stoll(ele->NextSiblingElement("value")->GetText());
        }
        else if (attName == "ssd.init.pe.cycles")
        {
            _ssd_init_pe_cycles = std::stoll(ele->NextSiblingElement("value")->GetText());
        }
        else if (attName == "ssd.init.retention.days")
        {
            _ssd_init_retention_days = std::stoll(ele->NextSiblingElement("value")->GetText());
        }
        else if (attName == "ssd.rber.model")
        {
            _ssd_rber_model = ele->NextSiblingElement("value")->GetText();
        }
        else if (attName == "ssd.errorlog.location")
        {
            _ssd_errorlog_location = ele->NextSiblingElement("value")->GetText();
        }
    }
}

Config::~Config()
{
}