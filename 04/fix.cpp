#include <map>
#include <functional>
#include <string>
#include <stack>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <memory>
#include <utility>

using namespace std;

struct FixEnv;
struct FixHandle;
struct FixObject{
    virtual bool isHandle(){
        return false;
    }
};

struct FixNum:FixObject{
    int value;
    FixNum(int i):value(i){}
};

using FixParam = vector<shared_ptr<FixObject>>;

struct FixStub:FixObject{
    size_t params;
    function<void(FixEnv&, FixParam&, FixHandle&)> fn;
    FixStub(function<void(FixEnv&, FixParam&, FixHandle&)> fn_, size_t params_):params(params_), fn(fn_){}
    FixStub(){};
};

struct FixHandle:FixObject{
    size_t params;
    FixStub* stub;
    virtual bool isHandle() override {
        return true;
    }
    vector<shared_ptr<FixObject>> slots;
    FixHandle(FixStub& stub_):params(stub_.params),
    stub(&stub_),slots(){};
    virtual void invoke(FixEnv& env){
        this->stub->fn(env, slots, *this);
    }
};
//// TODO
struct FixBlock:FixHandle{
    vector<string> cmds;
    FixBlock(FixStub& stub_):FixHandle(stub_){
    }
    virtual void invoke(FixEnv& env){

    }
};

struct FixEnv{
    static void regist(string name, size_t params, function<void(FixEnv&,FixParam&,FixHandle&)> fn){
        env[name] = FixStub(fn, params);
    }
    void eval(string s){
        if(env.find(s) == env.end()){ cout << "Error: Cannot found op `"<< s << "`!" <<endl; return;}
        auto handle = make_shared<FixHandle>(env[s]);
        while(!stk.empty() && handle->params){
            handle->slots.push_back(stk.top());
            handle->params--;
            stk.pop();
        }
        if(!handle->params)
            handle->invoke(*this);
        else
            stk.push(handle);
    }
    void eval(int i){
        if(!stk.empty() && stk.top()->isHandle()){
            auto handle = dynamic_pointer_cast<FixHandle>(stk.top());
            handle->slots.push_back(make_shared<FixNum>(i));
            handle->params--;
            if(!handle->params) {
                stk.pop();
                handle->invoke(*this);
            }
        }else{
            stk.push(make_shared<FixNum>(i));
        }
    }
    void ret(shared_ptr<FixObject> ptr){
        stk.push(ptr);
    }
    static void init_base();
private:
    stack<shared_ptr<FixObject>> stk;
    static map<string,FixStub> env;
};

map<string,FixStub> FixEnv::env{};

void FixEnv::init_base(){
    regist("fix", 1, [](FixEnv& fx,FixParam& $, FixHandle& self){
        cout <<"=> "<< dynamic_pointer_cast<FixNum>($[0])->value << endl;
        fx.ret($[0]);
    });
    regist("+", 2, [](FixEnv& fx,FixParam& $, FixHandle& self){
        fx.ret(
            make_shared<FixNum>(
                dynamic_pointer_cast<FixNum>($[0])->value +
                dynamic_pointer_cast<FixNum>($[1])->value
            ));
    });
    regist("-", 2, [](FixEnv& fx,FixParam& $, FixHandle& self){
        fx.ret(
            make_shared<FixNum>(
                dynamic_pointer_cast<FixNum>($[0])->value -
                dynamic_pointer_cast<FixNum>($[1])->value
            ));
    });
    regist("*", 2, [](FixEnv& fx,FixParam& $, FixHandle& self){
        fx.ret(
            make_shared<FixNum>(
                dynamic_pointer_cast<FixNum>($[0])->value *
                dynamic_pointer_cast<FixNum>($[1])->value
            ));
    });
    regist("/", 2, [](FixEnv& fx,FixParam& $, FixHandle& self){
        fx.ret(
            make_shared<FixNum>(
                dynamic_pointer_cast<FixNum>($[0])->value /
                dynamic_pointer_cast<FixNum>($[1])->value
            ));
    });
    regist("times", 3, [](FixEnv& fx, FixParam& $, FixHandle& self){
        auto handle = dynamic_pointer_cast<FixHandle>($[0]);
        handle->slots.push_back($[1]);
        auto time = dynamic_pointer_cast<FixNum>($[2])->value;
        while(time --){
            handle->invoke(fx);
        };
    });
    regist("consume", 0, [](FixEnv& fx, FixParam& $, FixHandle& self){
        while(!fx.stk.empty()){
            if(fx.stk.top()->isHandle()) cout << "[Fix]" << "  ";
            else cout << dynamic_pointer_cast<FixNum>(fx.stk.top())->value << "  ";
            fx.stk.pop();
        }
        cout << endl;
    });
    regist("dup", 1, [](FixEnv& fx, FixParam& $, FixHandle& self){
        fx.ret($[0]);
        fx.ret($[0]);
    });
    regist("swap", 2, [](FixEnv& fx, FixParam& $, FixHandle& self){
        fx.ret($[0]);
        fx.ret($[1]);
    });
}

int main(){
    FixEnv fx;
    FixEnv::init_base();
    string s;
    while(cin >> s){
        if(!s.size()) continue;
        if(s[0] >= '0' && s[0] <= '9'){
            fx.eval(atoi(s.c_str()));
        }else{
            fx.eval(s);
        }
    }
    return 0;
}

