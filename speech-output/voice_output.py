import serial
import subprocess
import time

# Change COM port if needed
ser = serial.Serial('COM8', 115200, timeout=1)
time.sleep(2)

last_spoken = ""

def speak_windows(text):
    command = f'Add-Type -AssemblyName System.Speech; ' \
              f'$voice = New-Object System.Speech.Synthesis.SpeechSynthesizer; ' \
              f'$voice.Speak("{text}")'
    subprocess.run(["powershell", "-Command", command], capture_output=True)

while True:
    try:
        line = ser.readline().decode(errors='ignore').strip()

        if line:
            print("Received:", line)

        # Extract message
        if "Message:" in line:
            msg = line.split("Message:")[-1].strip()

            # Reset when idle
            if msg == "No specific request.":
                last_spoken = ""
                continue

            # Speak only when gesture changes
            if msg != last_spoken:
                print("Speaking:", msg)
                speak_windows(msg)
                last_spoken = msg

    except KeyboardInterrupt:
        print("Stopped")
        break

    except Exception as e:
        print("Error:", e)