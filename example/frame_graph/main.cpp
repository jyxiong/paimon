#include "paimon/core/fg/frame_graph.h"

using namespace paimon;

int main() {
  FrameGraph fg;

  struct FGBuffer{
    struct Descriptor {
      size_t size;
    };

    void create(void*, const Descriptor&) {}
    void destroy(void*, const Descriptor&) {}
    void preRead(void*, const Descriptor&, uint32_t) {}
    void preWrite(void*, const Descriptor&, uint32_t) {}
  };

  struct ExamplePassData {
    NodeId res;
  };

  struct ReadPassData {
    NodeId inputRes;
  };

  struct WritePassData {
    NodeId outputRes;
  };

  // 创建第一个pass，创建一个资源
  auto& createData = fg.create_pass<ExamplePassData>(
    "CreatePass",
    [&](FrameGraph::Builder& builder, ExamplePassData& data){
      data.res = builder.create<FGBuffer>("ExampleResource", {1024});
    },
    [&](FrameGraphResources& resources, void*) {
      std::cout << "CreatePass executed\n";
    }
  );

  // 创建第二个pass，读取资源
  auto& readData = fg.create_pass<ReadPassData>(
    "ReadPass",
    [&](FrameGraph::Builder& builder, ReadPassData& data){
      data.inputRes = builder.read(createData.res);
    },
    [&](FrameGraphResources& resources, void*) {
      std::cout << "ReadPass executed\n";
    }
  );

  // 创建第三个pass，写入资源
  auto& writeData = fg.create_pass<WritePassData>(
    "WritePass",
    [&](FrameGraph::Builder& builder, WritePassData& data){
      data.outputRes = builder.write(createData.res);
    },
    [&](FrameGraphResources& resources, void*) {
      std::cout << "WritePass executed\n";
    }
  );

  // 检查资源的引用计数
  std::cout << "Resource ref count: " << fg.getResourceNode(createData.res).getRefCount() << std::endl;
  std::cout << "Resource readers count: " << fg.getResourceNode(createData.res).getReaders().size() << std::endl;
  std::cout << "Resource writers count: " << fg.getResourceNode(createData.res).getWriters().size() << std::endl;

  return 0;
}