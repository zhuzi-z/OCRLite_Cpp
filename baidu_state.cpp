#include "

enum class BaiduState {
    KEEP_PUBLIC = 0,
    CHANGE2PUBLIC = 1,
    KEEP_PRIVATE = 2,
    CHANGE2PRIVATE = 3
};

BaiduState baidu_states[] = {BaiduState::KEEP_PUBLIC,
                             BaiduState::CHANGE2PUBLIC,
                             BaiduState::KEEP_PRIVATE,
                             BaiduState::CHANGE2PRIVATE};
