#ifndef __LEARNING_GEM5_PART2_SIMPLE_MEMOBJ_HH__
#define __LEARNING_GEM5_PART2_SIMPLE_MEMOBJ_HH__

#include "mem/port.hh"
#include "params/SimpleMemobj.hh"
#include "sim/sim_object.hh"

namespace gem5
{

class SimpleMemobj : public SimObject
{
  private:
    class CPUSidePort : public ResponsePort
    {
      private:
        SimpleMemobj *owner;
        bool needRetry;
        PacketPtr blockedPacket;
      public:
        CPUSidePort(const std::string& name, SimpleMemobj *owner):
            ResponsePort(name), owner(owner), needRetry(false),
            blockedPacket(nullptr)
        { }
        
        void sendPacket(PacketPtr pkt);
        
        AddrRangeList getAddrRanges() const override;
        
        void trySendRetry();
        
      protected:
        Tick recvAtomic(Packet pkt) override
        { panic("recvAtomic unimpl.");}
        
        void recvFunctional(PacketPtr pkt) override;
        
        bool recvTimingReq(PacketPtr pkt) override;
        
        void recvRespRetry() override;
    };
    
    class MemSidePort:public RequestPort
    {
      private:
        SimpleMemobj *owner;
        PacketPtr blockedPacket;
      public:
        MemSidePort(const std::string& name, SimpleMemobj *owner):
            RequestPort(name), owner(owner), blockedPacket(nullptr)
        { }
        
        void sendPacket(PacketPtr pkt);
        
      protected:
        bool recvTimingResp(PacketPtr pkt) override;
        void recvReqRetry() override;
        void recvRangeChange() override;        
    };
    
    //handle the request from the cpu side
    bool handleRequest(PacketPtr pkt);
    //handle the response from the memory side
    bool handleResponse(PacketPtr pkt);
    //handle a packet functionally. update the data on a write and get the data on a read.
    void handleFunctional(PacketPtr pkt);
    //return the address ranges this memobj is responsible for.
    AddrRangeList getAddrRanges() const;
    
    void sendRangeChange();
    
    CPUSidePort instPost;
    CPUSidePort dataPort;
    
    MemSidePort memPort;
    
    bool blocked;
    
  public:
    //constructor
    SimpleMemobj(const SimpleMemobjParams &params);
    
    Port &getPort(const std::string &if_name, PortID idx=InvalidPortID) override;
    

};

}  //namespace gem5

#endif  // __LEARNING_GEM5_PART2_SIMPLE_MEMOBJ_HH__
