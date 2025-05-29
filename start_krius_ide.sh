#!/bin/bash

# Krius IDE Startup Script
# Starts BAGEL API server and launches the IDE

echo "🚀 Starting Krius IDE with BAGEL AI Integration..."

# Check if Python is available
if ! command -v python3 &> /dev/null; then
    echo "❌ Python 3 is required but not installed."
    exit 1
fi

# Check if required Python packages are installed
python3 -c "import fastapi, uvicorn, pydantic" 2>/dev/null
if [ $? -ne 0 ]; then
    echo "📦 Installing required Python packages..."
    pip3 install fastapi uvicorn pydantic pillow
fi

# Start BAGEL API server in background
echo "🤖 Starting BAGEL API server on port 12000..."
python3 bagel_api_server.py > bagel_server.log 2>&1 &
BAGEL_PID=$!

# Wait for server to start
sleep 3

# Check if server is running
curl -s http://localhost:12000/health > /dev/null
if [ $? -eq 0 ]; then
    echo "✅ BAGEL API server started successfully"
else
    echo "❌ Failed to start BAGEL API server"
    kill $BAGEL_PID 2>/dev/null
    exit 1
fi

# Check if IDE executable exists
if [ ! -f "build/bin/KriusIDE" ]; then
    echo "❌ KriusIDE executable not found. Please build the project first:"
    echo "   mkdir build && cd build && cmake .. && make"
    kill $BAGEL_PID 2>/dev/null
    exit 1
fi

# Launch IDE
echo "🎯 Launching Krius IDE..."
cd build
QT_QPA_PLATFORM=offscreen ./bin/KriusIDE &
IDE_PID=$!

echo "🎉 Krius IDE is now running!"
echo "   - BAGEL API Server: http://localhost:12000"
echo "   - IDE Process ID: $IDE_PID"
echo "   - BAGEL Process ID: $BAGEL_PID"
echo ""
echo "📝 Logs:"
echo "   - BAGEL Server: bagel_server.log"
echo "   - IDE Output: Check terminal"
echo ""
echo "🛑 To stop all services:"
echo "   kill $IDE_PID $BAGEL_PID"

# Create stop script
cat > stop_krius_ide.sh << EOF
#!/bin/bash
echo "🛑 Stopping Krius IDE services..."
kill $IDE_PID $BAGEL_PID 2>/dev/null
echo "✅ Services stopped"
EOF
chmod +x stop_krius_ide.sh

# Wait for IDE to exit
wait $IDE_PID

# Cleanup
echo "🧹 Cleaning up..."
kill $BAGEL_PID 2>/dev/null
echo "✅ Krius IDE session ended"