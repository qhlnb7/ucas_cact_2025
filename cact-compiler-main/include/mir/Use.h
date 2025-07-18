//
// Created by XuRuiyuan on 2023/7/14.
//

#ifndef FASTER_MEOW_USE_H
#define FASTER_MEOW_USE_H

#include <string>

// #include "Value.h"
// #include "Instr.h"
#include "../util/ILinkNode.h"

class Value;

class Instr;

class Use : public ILinkNode {
private:
    Value *used;
    int index;

public:
    static int use_num;  // Static member for counting
    Instr *user;  // Made public for compatibility
    
    Use();  // Default constructor
    Use(Value *used, Instr *user);
    Use(Instr *user, Value *used, int index);

    // 添加 Use 的析构函数
    virtual ~Use();

    Value *get_used() const;

    void set_used(Value *used);

    Instr *get_user() const;

    void set_user(Instr *user);
    
    int get_index();
    
    void set_index(int index);

    friend bool operator<(const Use &first, const Use &second);

    explicit operator std::string() const;
};

#endif //FASTER_MEOW_USE_H
