// Copyright (c) 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ipc/ipc_message_macros.h"
// 下面两次(或者3次)宏召开中，定义enum时在IPC_BEGIN_MESSAGES用到一个枚举
//变量TestMsgStart 在ipc_message_utils.h中定义

//ipc_enum 宏展开
/*
enum TestMsgType {
TestStart = TestMsgStart << 12,
TestPreStart = (TestMsgStart << 12) - 1,
SyncChannelTestMsg_NoArgs__ID,
SyncChannelTestMsg_AnswerToLife__ID,
SyncChannelTestMsg_Double__ID,
Msg_C_0_1__ID,
....,
Msg_R_3_2__ID,
Msg_R_3_3__ID,
TestEnd
};
*/
//ipc_classes宏展开
/*
class SyncChannelTestMsg_NoArgs{...};
class SyncChannelTestMsg_AnswerToLife{...};
...
class Msg_C_0_1 : public IPC::MessageWithReply<Tuple0, Tuple1<bool&> > {
public:
enum { ID = Msg_C_0_1__ID }; //看TestMsgType
Msg_C_0_1(bool* arg1)
: IPC::MessageWithReply<Tuple0, Tuple1<bool&> >(
MSG_ROUTING_CONTROL,
ID,
MakeTuple(), MakeRefTuple(*arg1)) {}
};
....
class Msg_R_3_2{...};
class Msg_R_3_3{...};
*/
/*
SYNC消息机制
消息映射宏的形式为IPC_SYNC_MESSAGE_CONTROL$(IN)_$(OUT)，表示的是同步消息，
意思是:当@A发送消息a给@B，@B接收到a之后，需要返回一个消息b给@A。
其中宏中的$IN和$OUT分别表示输入参数的个数
以及输出参数的个数，通过宏展开我们可以知道
*/
IPC_BEGIN_MESSAGES(Test)
IPC_SYNC_MESSAGE_CONTROL0_0(SyncChannelTestMsg_NoArgs)

IPC_SYNC_MESSAGE_CONTROL0_1(SyncChannelTestMsg_AnswerToLife,
int )

IPC_SYNC_MESSAGE_CONTROL1_1(SyncChannelTestMsg_Double,
int ,
int )

// out1 is false
IPC_SYNC_MESSAGE_CONTROL0_1(Msg_C_0_1, bool)
//经过ipc_enum宏展开变为
//Msg_C_0_1__ID
//经过ipc_class宏展开变为
/*******************************************************************
class Msg_C_0_1 : public IPC::MessageWithReply<Tuple0, Tuple1<bool&> > {
public:
enum { ID = Msg_C_0_1__ID };
Msg_C_0_1(bool* arg1)
: IPC::MessageWithReply<Tuple0, Tuple1<bool&> >(
MSG_ROUTING_CONTROL,
ID,
MakeTuple(), MakeRefTuple(*arg1)) {}
};
*******************************************************************/
// out1 is true, out2 is 2
IPC_SYNC_MESSAGE_CONTROL0_2(Msg_C_0_2, bool, int)

// out1 is false, out2 is 3, out3 is "0_3"
IPC_SYNC_MESSAGE_CONTROL0_3(Msg_C_0_3, bool, int, std::string)

// in1 must be 1, out1 is true
IPC_SYNC_MESSAGE_CONTROL1_1(Msg_C_1_1, int, bool)

// in1 must be false, out1 is true, out2 is 12
IPC_SYNC_MESSAGE_CONTROL1_2(Msg_C_1_2, bool, bool, int)

// in1 must be 3, out1 is "1_3", out2 is 13, out3 is false
IPC_SYNC_MESSAGE_CONTROL1_3(Msg_C_1_3, int, std::string, int, bool)

// in1 must be 1, in2 must be false, out1 is true
IPC_SYNC_MESSAGE_CONTROL2_1(Msg_C_2_1, int, bool, bool)

// in1 must be false, in2 must be 2, out1 is true, out2 is 22
IPC_SYNC_MESSAGE_CONTROL2_2(Msg_C_2_2, bool, int, bool, int)

// in1 must be 3, in2 must be true, out1 is "2_3", out2 is 23, out3 is false
IPC_SYNC_MESSAGE_CONTROL2_3(Msg_C_2_3, int, bool, std::string, int, bool)

// in1 must be 1, in2 must be false, in3 must be "3_1", out1 is true
IPC_SYNC_MESSAGE_CONTROL3_1(Msg_C_3_1, int, bool, std::string, bool)

// in1 must be "3_3", in2 must be false, in3 must be 2, out1 is true, out2 is
// 32
IPC_SYNC_MESSAGE_CONTROL3_2(Msg_C_3_2, std::string, bool, int, bool, int)

// in1 must be 3, in2 must be "3_3", in3 must be true, out1 is "3_3", out2 is
// 33, out3 is false
IPC_SYNC_MESSAGE_CONTROL3_3(Msg_C_3_3, int, std::string, bool, std::string,
int, bool)


// NOTE: routed messages are just a copy of the above...

// out1 is false
IPC_SYNC_MESSAGE_ROUTED0_1(Msg_R_0_1, bool)

// out1 is true, out2 is 2
IPC_SYNC_MESSAGE_ROUTED0_2(Msg_R_0_2, bool, int)

// out1 is false, out2 is 3, out3 is "0_3"
IPC_SYNC_MESSAGE_ROUTED0_3(Msg_R_0_3, bool, int, std::string)

// in1 must be 1, out1 is true
IPC_SYNC_MESSAGE_ROUTED1_1(Msg_R_1_1, int, bool)

// in1 must be false, out1 is true, out2 is 12
IPC_SYNC_MESSAGE_ROUTED1_2(Msg_R_1_2, bool, bool, int)

// in1 must be 3, out1 is "1_3", out2 is 13, out3 is false
IPC_SYNC_MESSAGE_ROUTED1_3(Msg_R_1_3, int, std::string, int, bool)

// in1 must be 1, in2 must be false, out1 is true
IPC_SYNC_MESSAGE_ROUTED2_1(Msg_R_2_1, int, bool, bool)

// in1 must be false, in2 must be 2, out1 is true, out2 is 22
IPC_SYNC_MESSAGE_ROUTED2_2(Msg_R_2_2, bool, int, bool, int)

// in1 must be 3, in2 must be true, out1 is "2_3", out2 is 23, out3 is false
IPC_SYNC_MESSAGE_ROUTED2_3(Msg_R_2_3, int, bool, std::string, int, bool)

// in1 must be 1, in2 must be false, in3 must be "3_1", out1 is true
IPC_SYNC_MESSAGE_ROUTED3_1(Msg_R_3_1, int, bool, std::string, bool)

// in1 must be "3_3", in2 must be false, in3 must be 2, out1 is true, out2
// is 32
IPC_SYNC_MESSAGE_ROUTED3_2(Msg_R_3_2, std::string, bool, int, bool, int)

// in1 must be 3, in2 must be "3_3", in3 must be true, out1 is "3_3", out2 is
// 33, out3 is false
IPC_SYNC_MESSAGE_ROUTED3_3(Msg_R_3_3, int, std::string, bool, std::string,
int, bool)

IPC_END_MESSAGES(TestMsg)
