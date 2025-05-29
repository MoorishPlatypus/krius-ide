#!/usr/bin/env python3
"""
Test script to verify BAGEL integration with Krius IDE
"""

import requests
import json

def test_bagel_server():
    """Test BAGEL server health and functionality"""
    base_url = "http://localhost:12000"
    
    print("🔍 Testing BAGEL Server Integration...")
    
    # Test health endpoint
    try:
        response = requests.get(f"{base_url}/health")
        if response.status_code == 200:
            health_data = response.json()
            print(f"✅ Health Check: {health_data['status']}")
            print(f"✅ Model Loaded: {health_data['model_loaded']}")
        else:
            print(f"❌ Health check failed: {response.status_code}")
            return False
    except Exception as e:
        print(f"❌ Health check error: {e}")
        return False
    
    # Test code generation
    try:
        code_request = {
            "prompt": "Create a simple C++ hello world program",
            "language": "cpp"
        }
        response = requests.post(f"{base_url}/generate_code", json=code_request)
        if response.status_code == 200:
            result = response.json()
            print("✅ Code Generation Test:")
            print(f"   Generated: {result['code'][:100]}...")
        else:
            print(f"❌ Code generation failed: {response.status_code}")
    except Exception as e:
        print(f"❌ Code generation error: {e}")
    
    # Test code explanation
    try:
        explain_request = {
            "code": "#include <iostream>\nint main() { std::cout << \"Hello\" << std::endl; return 0; }",
            "language": "cpp"
        }
        response = requests.post(f"{base_url}/explain_code", json=explain_request)
        if response.status_code == 200:
            result = response.json()
            print("✅ Code Explanation Test:")
            print(f"   Explanation: {result['explanation'][:100]}...")
        else:
            print(f"❌ Code explanation failed: {response.status_code}")
    except Exception as e:
        print(f"❌ Code explanation error: {e}")
    
    # Test chat
    try:
        chat_request = {
            "message": "What is the best way to learn C++ programming?"
        }
        response = requests.post(f"{base_url}/chat", json=chat_request)
        if response.status_code == 200:
            result = response.json()
            print("✅ Chat Test:")
            print(f"   Response: {result['response'][:100]}...")
        else:
            print(f"❌ Chat failed: {response.status_code}")
    except Exception as e:
        print(f"❌ Chat error: {e}")
    
    print("\n🎉 BAGEL Integration Test Complete!")
    return True

if __name__ == "__main__":
    test_bagel_server()