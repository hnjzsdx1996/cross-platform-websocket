# 跨平台 WebSocket 框架架构文档

本文档包含了跨平台 WebSocket 框架的详细架构设计，使用 PlantUML 绘制了多种类型的图表来展示系统的不同方面。

## 图表说明

### 1. 详细类图 (class_diagram.puml)

**文件**: `docs/class_diagram.puml`

这个类图展示了整个框架的详细类结构，包括：

- **外部 API 层**: Java、Python、Go、JavaScript、C++ 客户端
- **平台桥接层**: JNI、SWIG、CGO、Emscripten 桥接
- **C API 层**: 统一的 C 风格接口
- **C++ API 层**: 面向对象的 C++ 接口
- **业务管理层**: 连接管理、消息管理、事件管理、配置管理
- **核心组件层**: 数据链路、日志、线程池、工具类
- **平台抽象层**: 平台能力注入接口

**特点**:
- 使用不同颜色区分各个层次
- 展示了接口实现关系
- 包含了详细的方法签名
- 显示了组件间的依赖关系

### 2. 简化架构图 (simple_architecture.puml)

**文件**: `docs/simple_architecture.puml`

这个简化类图提供了更清晰的架构概览：

- **外部 API 层**: 各种语言的客户端类
- **平台桥接层**: 语言特定的桥接类
- **API 层**: C 和 C++ API 类
- **业务层**: 核心业务逻辑类
- **核心组件层**: 基础功能接口和类
- **平台抽象层**: 平台能力抽象接口和实现

**特点**:
- 简化的类结构
- 突出核心架构
- 清晰的层次关系
- 便于快速理解

### 3. 组件图 (component_diagram.puml)

**文件**: `docs/component_diagram.puml`

这个组件图提供了更高层次的架构视图，专注于组件间的依赖关系：

- **外部 API 层**: 各种语言的客户端组件
- **平台桥接层**: 语言特定的桥接组件
- **API 层**: C 和 C++ API 组件
- **业务管理层**: 核心业务逻辑组件
- **核心组件层**: 基础功能组件
- **平台抽象层**: 平台能力抽象组件
- **实现层**: 底层实现组件

**特点**:
- 清晰的层次结构
- 简化的依赖关系
- 便于理解整体架构
- 突出核心组件

### 4. 序列图 (sequence_diagram.puml)

**文件**: `docs/sequence_diagram.puml`

这个序列图展示了 WebSocket 操作的完整流程：

- **初始化阶段**: 创建上下文和初始化各个组件
- **连接阶段**: 建立 WebSocket 连接
- **发送消息阶段**: 发送消息到服务器
- **接收消息阶段**: 接收服务器消息
- **断开连接阶段**: 关闭 WebSocket 连接
- **清理阶段**: 释放资源和清理

**特点**:
- 详细的操作流程
- 展示了各层之间的交互
- 包含了回调机制
- 完整的生命周期管理

### 5. 数据流图 (data_flow.puml)

**文件**: `docs/data_flow.puml`

这个活动图展示了数据在系统中的处理流程：

- **处理层次**: 从外部客户端到 WebSocket 服务器的处理层次
- **各层职责**: 展示了每一层的具体职责
- **处理顺序**: 清晰的处理流程

**特点**:
- 简化的处理流程
- 清晰的层次结构
- 各层职责说明
- 便于理解处理顺序

### 6. 数据流序列图 (data_flow_sequence.puml)

**文件**: `docs/data_flow_sequence.puml`

这个序列图详细展示了消息在系统中的流动过程：

- **请求流**: 从外部客户端到 WebSocket 服务器的数据流
- **响应流**: 从 WebSocket 服务器到外部客户端的回调流
- **各层处理**: 展示了数据在每一层的处理过程

**特点**:
- 清晰的数据流向
- 双向通信展示
- 各层职责说明
- 便于理解消息处理流程

### 7. 部署图 (deployment_diagram.puml)

**文件**: `docs/deployment_diagram.puml`

这个部署图展示了不同平台的部署架构：

- **Android 平台**: Java 客户端 + JNI + 原生库
- **iOS 平台**: Swift/ObjC 客户端 + C++ 桥接 + 原生库
- **Desktop 平台**: C++ 客户端 + 原生库 + libwebsockets
- **Web 平台**: JavaScript 客户端 + Emscripten + WebAssembly
- **Server 平台**: Python/Go/Java/C++ 客户端 + 运行时

**特点**:
- 平台特定的部署结构
- 共享核心库的概念
- 运行时环境展示
- 跨平台一致性

## 如何使用这些图表

### 查看图表

1. **在线查看**: 将 `.puml` 文件内容复制到 [PlantUML 在线编辑器](http://www.plantuml.com/plantuml/uml/)
2. **本地工具**: 使用支持 PlantUML 的 IDE 插件
3. **命令行**: 使用 PlantUML 命令行工具生成图片

### 生成图片

```bash
# 安装 PlantUML
# macOS
brew install plantuml

# 生成图片
plantuml docs/class_diagram.puml
plantuml docs/simple_architecture.puml
plantuml docs/component_diagram.puml
plantuml docs/sequence_diagram.puml
plantuml docs/data_flow.puml
plantuml docs/data_flow_sequence.puml
plantuml docs/deployment_diagram.puml
```

### 在文档中使用

```markdown
![详细类图](docs/class_diagram.png)
![简化架构图](docs/simple_architecture.png)
![组件图](docs/component_diagram.png)
![序列图](docs/sequence_diagram.png)
![数据流图](docs/data_flow.png)
![数据流序列图](docs/data_flow_sequence.png)
![部署图](docs/deployment_diagram.png)
```

## 图表选择指南

### 不同场景下的图表选择

1. **整体架构理解**: 使用 `simple_architecture.puml`
2. **详细设计实现**: 使用 `class_diagram.puml`
3. **组件关系分析**: 使用 `component_diagram.puml`
4. **操作流程理解**: 使用 `sequence_diagram.puml`
5. **数据流向分析**: 使用 `data_flow_sequence.puml`
6. **处理流程理解**: 使用 `data_flow.puml`
7. **部署规划**: 使用 `deployment_diagram.puml`

### 图表复杂度对比

| 图表类型 | 复杂度 | 适用场景 | 主要用途 |
|---------|--------|----------|----------|
| 简化架构图 | 低 | 快速理解 | 架构概览 |
| 数据流图 | 低 | 流程理解 | 处理顺序 |
| 数据流序列图 | 中 | 数据流向 | 消息流动 |
| 组件图 | 中 | 组件分析 | 依赖关系 |
| 序列图 | 中 | 交互理解 | 操作流程 |
| 部署图 | 中 | 部署规划 | 平台部署 |
| 详细类图 | 高 | 详细设计 | 实现细节 |

## 架构设计原则

### 1. 分层架构
- 清晰的层次划分
- 单向依赖关系
- 接口抽象

### 2. 跨平台支持
- 统一的 C API
- 平台特定的桥接
- 能力注入机制

### 3. 模块化设计
- 组件化架构
- 松耦合设计
- 高内聚模块

### 4. 可扩展性
- 插件化架构
- 配置驱动
- 事件驱动

## 核心特性

### 1. 多语言支持
- Java (JNI)
- Python (SWIG)
- Go (CGO)
- JavaScript (Emscripten)
- C++ (直接使用)

### 2. 多平台支持
- Android
- iOS
- Desktop (Windows/macOS/Linux)
- Web Browser
- Server

### 3. 核心功能
- WebSocket 客户端/服务端
- SSL/TLS 支持
- 连接管理
- 消息收发
- 事件处理
- 日志系统
- 线程池
- 配置管理

### 4. 性能优化
- 异步 I/O
- 线程池管理
- 内存池
- 零拷贝优化

## 开发指南

### 1. 添加新语言支持
1. 在 `platform/` 目录下创建新的语言目录
2. 实现语言特定的桥接代码
3. 更新构建脚本
4. 添加示例代码

### 2. 添加新平台支持
1. 在 `platform/` 目录下创建新的平台目录
2. 实现平台特定的能力注入
3. 更新部署配置
4. 添加平台测试

### 3. 扩展核心功能
1. 在 `src/core/` 目录下添加新组件
2. 实现相应的接口
3. 更新业务管理层
4. 添加单元测试

## 维护说明

### 1. 图表更新
- 修改代码时同步更新相关图表
- 保持图表与代码的一致性
- 定期审查架构设计

### 2. 文档维护
- 及时更新 API 文档
- 保持示例代码的最新状态
- 定期检查文档的准确性

### 3. 版本管理
- 使用语义化版本号
- 记录重要的架构变更
- 维护变更日志

这些图表为跨平台 WebSocket 框架提供了完整的架构视图，帮助开发者理解系统的设计思路和实现细节。根据不同的需求，可以选择合适的图表来理解系统的不同方面。 