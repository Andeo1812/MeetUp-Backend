#include <nlohmann/json.hpp>

#include "ParserGeneralData.hpp"

Context ParserGeneralData::StrToObject(const std::string &parser_str) const {
    nlohmann::json j = nlohmann::json::parse(parser_str);

    nlohmann::json value = j[j.begin().key()];

    GeneralData general_data;

    get_data<std::string>(&GeneralData::SetUserId,        &general_data, value, "user_id");
    get_data<std::string>(&GeneralData::SetStatus,        &general_data, value, "status");
    get_data<std::string>(&GeneralData::SetTags,          &general_data, value, "tags");
    get_data<std::string>(&GeneralData::SetDescription,   &general_data, value, "description");
    get_data<std::string>(&GeneralData::SetNickname,      &general_data, value, "nickname");

    Context res = general_data;

    return res;
}

std::string ParserGeneralData::ObjectToStr(const std::string type_response, const Context &other) const {
    nlohmann::json j;

    std::string res;

    if (!other.GetError().empty()) {
        j[type_response] = other.GetError();

        res = j.dump();

        return res;
    }

    if (type_response == WRITE_GENERAL_DATA) {
        j[type_response] = "OK";

        res = j.dump();

        return res;
    }

    GeneralData general_data = other.GetGeneralData();

    nlohmann::json value;

    get_json(&GeneralData::GetUserId,       general_data, &value, "user_id");
    get_json(&GeneralData::GetStatus,       general_data, &value, "status");
    get_json(&GeneralData::GetTags,         general_data, &value, "tags");
    get_json(&GeneralData::GetDescription,  general_data, &value, "description");
    get_json(&GeneralData::GetNickname,     general_data, &value, "nickname");

    j[type_response] = value;

    res = j.dump();

    return res;
}
