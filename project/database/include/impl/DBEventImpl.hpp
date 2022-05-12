#pragma once  //  NOLINT

#include "DBEvent.hpp"

static enum result_add_event {NOT_ADD_EVENT = 0, ERROR_ADD_EVENT} GET_ADD_EVENT;

static enum result_rm_event {NOT_RM_EVENT = 5, ERROR_RM_EVENT} RM_EVENT;

class DBEventImpl : public DBEvent {
 public:
    int Add(const Event &event, std::string &new_event_id) const override;

    int Write(const Event &event) const override;

    int Rm(const Event &event) const override;

    int GetId(const Event &event, std::string &event_id) const override;

    DBEventImpl() = default;
    ~DBEventImpl() = default;
};
