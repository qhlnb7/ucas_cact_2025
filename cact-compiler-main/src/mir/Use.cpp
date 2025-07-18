//
// Created by XuRuiyuan on 2023/7/14.
//

#include "../../include/mir/Use.h"
#include <string>
#include <sstream>
#include <cassert>

#include "Value.h"
#include "Instr.h"

Use::Use() : used(nullptr), user(nullptr), index(0) {}

Use::Use(Value *used, Instr *user) : used(used), user(user), index(0) {}

Use::Use(Instr *user, Value *used, int index) : user(user), used(used), index(index) {}

Use::~Use() {}

Value *Use::get_used() const {
    return used;
}

void Use::set_used(Value *used) {
    this->used = used;
}

Instr *Use::get_user() const {
    return user;
}

void Use::set_user(Instr *user) {
    this->user = user;
}

int Use::get_index() {
    return index;
}

void Use::set_index(int index) {
    this->index = index;
}

bool operator<(const Use &first, const Use &second) {
    return first.get_used() < second.get_used();
}

Use::operator std::string() const {
    std::stringstream ss;
    assert(this->used != nullptr && this->user != nullptr);
    ss << "Use(" << used << " @ " << user << ")";
    return ss.str();
}