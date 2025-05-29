#!/usr/bin/env python3
"""
BAGEL API Server for Krius IDE Integration
Provides REST API endpoints for BAGEL multimodal AI functionality
"""

from fastapi import FastAPI, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from pydantic import BaseModel
from typing import Optional, List
import uvicorn
import base64
import io
from PIL import Image
import json

# Mock BAGEL functionality for demonstration
# In a real implementation, this would integrate with the actual BAGEL model

app = FastAPI(title="BAGEL API Server", description="BAGEL AI Integration for Krius IDE")

# Enable CORS for web integration
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# Request/Response Models
class CodeGenerationRequest(BaseModel):
    prompt: str
    language: str = "cpp"
    max_length: Optional[int] = 512

class CodeGenerationResponse(BaseModel):
    code: str
    language: str
    explanation: Optional[str] = None

class CodeExplanationRequest(BaseModel):
    code: str
    language: str = "cpp"

class CodeExplanationResponse(BaseModel):
    explanation: str
    suggestions: Optional[List[str]] = None

class ChatRequest(BaseModel):
    message: str
    context: Optional[str] = None

class ChatResponse(BaseModel):
    response: str
    context: Optional[str] = None

class ImageGenerationRequest(BaseModel):
    prompt: str
    width: Optional[int] = 512
    height: Optional[int] = 512

class ImageGenerationResponse(BaseModel):
    image_base64: str
    prompt: str

class HealthResponse(BaseModel):
    status: str
    model_loaded: bool
    service: str

# Global state
model_loaded = True

@app.get("/health", response_model=HealthResponse)
async def health_check():
    """Health check endpoint"""
    return HealthResponse(
        status="healthy",
        model_loaded=model_loaded,
        service="BAGEL API Server"
    )

@app.post("/generate_code", response_model=CodeGenerationResponse)
async def generate_code(request: CodeGenerationRequest):
    """Generate code from natural language prompt"""
    try:
        # Mock code generation based on language and prompt
        if request.language.lower() in ["cpp", "c++"]:
            if "hello world" in request.prompt.lower():
                code = """#include <iostream>

int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}"""
            elif "class" in request.prompt.lower():
                code = """#include <iostream>
#include <string>

class MyClass {
private:
    std::string name;
    int value;

public:
    MyClass(const std::string& n, int v) : name(n), value(v) {}
    
    void display() const {
        std::cout << "Name: " << name << ", Value: " << value << std::endl;
    }
    
    // Getters and setters
    const std::string& getName() const { return name; }
    int getValue() const { return value; }
    void setValue(int v) { value = v; }
};

int main() {
    MyClass obj("Example", 42);
    obj.display();
    return 0;
}"""
            else:
                code = f"""// Generated C++ code for: {request.prompt}
#include <iostream>

int main() {{
    // TODO: Implement {request.prompt}
    std::cout << "Implementation needed" << std::endl;
    return 0;
}}"""
        
        elif request.language.lower() == "python":
            if "hello world" in request.prompt.lower():
                code = 'print("Hello, World!")'
            else:
                code = f"""# Generated Python code for: {request.prompt}

def main():
    # TODO: Implement {request.prompt}
    print("Implementation needed")

if __name__ == "__main__":
    main()"""
        
        else:
            code = f"// Generated {request.language} code for: {request.prompt}\n// TODO: Implement functionality"
        
        explanation = f"This code was generated based on your prompt: '{request.prompt}'. It provides a basic structure that you can extend and modify as needed."
        
        return CodeGenerationResponse(
            code=code,
            language=request.language,
            explanation=explanation
        )
    
    except Exception as e:
        raise HTTPException(status_code=500, detail=f"Code generation failed: {str(e)}")

@app.post("/explain_code", response_model=CodeExplanationResponse)
async def explain_code(request: CodeExplanationRequest):
    """Explain provided code"""
    try:
        # Mock code explanation
        code_lines = len(request.code.split('\n'))
        
        if request.language.lower() in ["cpp", "c++"]:
            explanation = f"""This C++ code contains {code_lines} lines. Here's what it does:

1. **Headers**: The code includes necessary header files for input/output operations
2. **Main Function**: The entry point of the program where execution begins
3. **Logic**: The core functionality is implemented within the main function
4. **Return Statement**: The program returns 0 to indicate successful execution

**Key C++ Concepts Used:**
- Standard library usage
- Function definitions
- Variable declarations
- Control flow structures"""

            suggestions = [
                "Consider adding error handling for robustness",
                "Add comments to explain complex logic",
                "Use const correctness where applicable",
                "Consider using modern C++ features (C++11/14/17/20)",
                "Add input validation for user inputs"
            ]
        
        elif request.language.lower() == "python":
            explanation = f"""This Python code contains {code_lines} lines. Here's the analysis:

1. **Structure**: The code follows Python conventions and syntax
2. **Functionality**: Implements the core logic using Python's built-in features
3. **Style**: Uses Python's readable and concise syntax

**Python Features Used:**
- Built-in functions and methods
- Python data structures
- Control flow statements
- Function definitions"""

            suggestions = [
                "Add type hints for better code documentation",
                "Use docstrings to document functions",
                "Consider using list comprehensions where appropriate",
                "Add error handling with try-except blocks",
                "Follow PEP 8 style guidelines"
            ]
        
        else:
            explanation = f"This {request.language} code contains {code_lines} lines. The code structure appears to follow standard conventions for the language."
            suggestions = ["Add appropriate comments", "Consider code organization", "Add error handling"]
        
        return CodeExplanationResponse(
            explanation=explanation,
            suggestions=suggestions
        )
    
    except Exception as e:
        raise HTTPException(status_code=500, detail=f"Code explanation failed: {str(e)}")

@app.post("/chat", response_model=ChatResponse)
async def chat(request: ChatRequest):
    """Chat with BAGEL AI assistant"""
    try:
        # Mock chat responses
        message_lower = request.message.lower()
        
        if "hello" in message_lower or "hi" in message_lower:
            response = "Hello! I'm BAGEL, your AI programming assistant. I can help you with code generation, explanation, debugging, and general programming questions. How can I assist you today?"
        
        elif "c++" in message_lower or "cpp" in message_lower:
            response = """C++ is a powerful, general-purpose programming language. Here are some key points:

**Strengths:**
- High performance and efficiency
- Object-oriented programming support
- Rich standard library
- Memory management control
- Wide industry adoption

**Learning Path:**
1. Start with basic syntax and data types
2. Learn about functions and control structures
3. Master object-oriented concepts (classes, inheritance, polymorphism)
4. Understand memory management (pointers, references)
5. Explore STL (Standard Template Library)
6. Practice with real projects

**Best Practices:**
- Use RAII (Resource Acquisition Is Initialization)
- Prefer smart pointers over raw pointers
- Follow const correctness
- Use modern C++ features (C++11 and later)

Would you like me to help you with any specific C++ topic?"""
        
        elif "python" in message_lower:
            response = """Python is an excellent language for beginners and professionals alike:

**Why Python:**
- Easy to learn and read
- Extensive libraries and frameworks
- Great for data science, web development, automation
- Strong community support

**Learning Resources:**
- Official Python tutorial
- Practice on coding platforms
- Build real projects
- Join Python communities

**Key Libraries:**
- NumPy/Pandas for data analysis
- Django/Flask for web development
- TensorFlow/PyTorch for machine learning
- Requests for HTTP operations

What specific Python topic interests you?"""
        
        elif "help" in message_lower or "what can you do" in message_lower:
            response = """I can assist you with various programming tasks:

🔧 **Code Generation**: Create code from natural language descriptions
📖 **Code Explanation**: Analyze and explain existing code
🎨 **Image Generation**: Create images from text descriptions
💬 **Programming Chat**: Answer questions about programming concepts
🐛 **Debugging Help**: Assist with troubleshooting code issues
📚 **Learning Guidance**: Provide learning resources and best practices

**Supported Languages:**
- C++ (primary focus)
- Python
- JavaScript
- And more!

Just ask me anything programming-related, and I'll do my best to help!"""
        
        elif "debug" in message_lower or "error" in message_lower:
            response = """I'd be happy to help you debug your code! Here's how I can assist:

**Debugging Process:**
1. **Share your code** - Paste the problematic code
2. **Describe the issue** - What error are you getting?
3. **Expected vs Actual** - What should happen vs what's happening?

**Common C++ Issues:**
- Segmentation faults (memory access errors)
- Compilation errors (syntax, missing headers)
- Logic errors (incorrect algorithm)
- Memory leaks (improper memory management)

**Debugging Tips:**
- Use a debugger (GDB, Visual Studio debugger)
- Add print statements to trace execution
- Check variable values at different points
- Validate input data
- Review error messages carefully

Please share your code and describe the specific problem you're facing!"""
        
        else:
            response = f"""I understand you're asking about: "{request.message}"

As your AI programming assistant, I'm here to help with:
- Code generation and explanation
- Programming concepts and best practices
- Debugging and troubleshooting
- Learning resources and guidance
- Technical questions

Could you provide more specific details about what you'd like help with? For example:
- What programming language are you working with?
- What specific problem are you trying to solve?
- Do you have any code you'd like me to review?

I'm ready to assist you with your programming journey!"""
        
        return ChatResponse(
            response=response,
            context=request.context
        )
    
    except Exception as e:
        raise HTTPException(status_code=500, detail=f"Chat failed: {str(e)}")

@app.post("/generate_image", response_model=ImageGenerationResponse)
async def generate_image(request: ImageGenerationRequest):
    """Generate image from text prompt"""
    try:
        # Create a simple placeholder image
        img = Image.new('RGB', (request.width, request.height), color='lightblue')
        
        # Convert to base64
        buffer = io.BytesIO()
        img.save(buffer, format='PNG')
        img_base64 = base64.b64encode(buffer.getvalue()).decode()
        
        return ImageGenerationResponse(
            image_base64=img_base64,
            prompt=request.prompt
        )
    
    except Exception as e:
        raise HTTPException(status_code=500, detail=f"Image generation failed: {str(e)}")

@app.get("/")
async def root():
    """Root endpoint with API information"""
    return {
        "service": "BAGEL API Server",
        "version": "1.0.0",
        "description": "REST API for BAGEL AI integration with Krius IDE",
        "endpoints": {
            "/health": "Health check",
            "/generate_code": "Generate code from prompt",
            "/explain_code": "Explain provided code",
            "/chat": "Chat with AI assistant",
            "/generate_image": "Generate image from prompt"
        }
    }

if __name__ == "__main__":
    uvicorn.run(
        app,
        host="0.0.0.0",
        port=12000,
        log_level="info"
    )