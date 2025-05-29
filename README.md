# Krius IDE - C++ IDE with BAGEL AI Integration

A modern C++ Integrated Development Environment with multimodal AI assistance powered by BAGEL.

## Features

### 🎯 Core IDE Features
- **Modern Qt5-based Interface**: Dark theme optimized for development
- **Advanced Code Editor**: Syntax highlighting, line numbers, auto-indentation
- **Multi-language Support**: C++, Python, JavaScript syntax highlighting
- **Project Management**: File explorer, project organization
- **Tabbed Interface**: Multiple files open simultaneously
- **Dockable Panels**: Customizable workspace layout

### 🤖 BAGEL AI Integration
- **Code Generation**: Generate code from natural language descriptions
- **Code Explanation**: AI-powered code analysis and documentation
- **Intelligent Chat**: Programming assistant for questions and guidance
- **Image Generation**: Create diagrams and visual aids (planned)
- **Real-time Assistance**: Integrated AI chat panel

### 🛠️ Development Tools
- **Build System**: Integrated compilation and build management
- **File Operations**: New, Open, Save, Save As functionality
- **Status Bar**: Real-time development status
- **Output Panel**: Build results and debugging information

## Architecture

### C++ Core Components
- **MainWindow**: Primary application interface with menus and toolbars
- **CodeEditor**: Advanced text editor with syntax highlighting
- **SyntaxHighlighter**: Multi-language syntax highlighting engine
- **ProjectManager**: File system integration and project management
- **BagelClient**: HTTP client for AI API communication
- **BagelChatWidget**: AI interaction interface

### BAGEL API Server
- **FastAPI Backend**: RESTful API for AI functionality
- **Code Generation**: `/generate_code` endpoint
- **Code Explanation**: `/explain_code` endpoint
- **Chat Interface**: `/chat` endpoint
- **Health Monitoring**: `/health` endpoint

## Build Instructions

### Prerequisites
```bash
# Install Qt5 development libraries
sudo apt-get update
sudo apt-get install qt5-default qtbase5-dev qttools5-dev-tools

# Install CMake and build tools
sudo apt-get install cmake build-essential

# Install Python dependencies for BAGEL server
pip install fastapi uvicorn pydantic pillow
```

### Compilation
```bash
# Clone and build
cd krius-cpp-ide
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### Running
```bash
# Start BAGEL API server
python bagel_api_server.py &

# Launch IDE
./bin/KriusIDE
```

## Usage

### Getting Started
1. **Launch the IDE**: Run the KriusIDE executable
2. **Open a Project**: File → Open Project or use Ctrl+Shift+O
3. **Create New File**: File → New or use Ctrl+N
4. **AI Assistance**: Use the BAGEL chat panel for code help

### AI Features
- **Generate Code**: Ask BAGEL to create code snippets
- **Explain Code**: Select code and ask for explanations
- **Programming Help**: Chat with AI for guidance and best practices
- **Debug Assistance**: Get help troubleshooting issues

### Keyboard Shortcuts
- `Ctrl+N`: New file
- `Ctrl+O`: Open file
- `Ctrl+S`: Save file
- `Ctrl+Shift+S`: Save as
- `Ctrl+Shift+O`: Open project
- `F5`: Build project
- `Ctrl+F5`: Run project

## API Documentation

### BAGEL Server Endpoints

#### Health Check
```http
GET /health
```
Returns server status and model availability.

#### Code Generation
```http
POST /generate_code
Content-Type: application/json

{
  "prompt": "Create a C++ class for a simple calculator",
  "language": "cpp",
  "max_length": 512
}
```

#### Code Explanation
```http
POST /explain_code
Content-Type: application/json

{
  "code": "#include <iostream>\nint main() { return 0; }",
  "language": "cpp"
}
```

#### Chat
```http
POST /chat
Content-Type: application/json

{
  "message": "How do I implement a binary search tree in C++?",
  "context": "optional context"
}
```

## Configuration

### IDE Settings
- Theme: Dark theme enabled by default
- Font: Monospace font for code editor
- Syntax: Automatic language detection

### BAGEL Server
- Host: localhost (configurable)
- Port: 12000 (configurable)
- Timeout: 30 seconds for API calls

## Development

### Project Structure
```
krius-cpp-ide/
├── src/                    # C++ source files
│   ├── main.cpp           # Application entry point
│   ├── MainWindow.cpp     # Main window implementation
│   ├── CodeEditor.cpp     # Code editor widget
│   ├── SyntaxHighlighter.cpp  # Syntax highlighting
│   ├── BagelClient.cpp    # AI API client
│   ├── BagelChatWidget.cpp    # AI chat interface
│   └── ProjectManager.cpp # Project management
├── include/               # Header files
├── resources/             # UI resources and icons
├── build/                 # Build output directory
├── bagel_api_server.py   # BAGEL API server
└── CMakeLists.txt        # Build configuration
```

### Adding Features
1. **New UI Components**: Add to MainWindow layout
2. **Editor Features**: Extend CodeEditor class
3. **AI Capabilities**: Enhance BagelClient and server endpoints
4. **Language Support**: Extend SyntaxHighlighter

## Testing

### Unit Tests
```bash
# Run BAGEL integration tests
python test_bagel_integration.py

# Test IDE compilation
cd build && make test
```

### Manual Testing
1. Code editing and syntax highlighting
2. File operations (new, open, save)
3. Project management
4. AI code generation and explanation
5. Chat functionality

## Deployment

### Packaging for Distribution
```bash
# Create distribution package
mkdir krius-ide-dist
cp build/bin/KriusIDE krius-ide-dist/
cp bagel_api_server.py krius-ide-dist/
cp -r resources krius-ide-dist/

# Create startup script
echo '#!/bin/bash
python bagel_api_server.py &
./KriusIDE' > krius-ide-dist/start.sh
chmod +x krius-ide-dist/start.sh
```

### Windows Deployment
For Windows deployment, use Qt's windeployqt tool to package dependencies.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Implement changes with tests
4. Submit a pull request

## License

This project combines:
- **VS Code**: MIT License (Microsoft)
- **BAGEL**: Apache 2.0 License (ByteDance)
- **Krius IDE**: MIT License

## Support

For issues and questions:
- GitHub Issues: Report bugs and feature requests
- Documentation: Check README and code comments
- Community: Join development discussions

## Roadmap

### Planned Features
- [ ] Advanced debugging integration
- [ ] Git version control integration
- [ ] Plugin system for extensions
- [ ] Multi-language project support
- [ ] Advanced AI features (image understanding)
- [ ] Code completion and IntelliSense
- [ ] Integrated terminal
- [ ] Theme customization

### Performance Improvements
- [ ] Faster syntax highlighting
- [ ] Optimized file loading
- [ ] Reduced memory usage
- [ ] Better AI response caching

---

**Krius IDE** - Empowering developers with AI-assisted programming.