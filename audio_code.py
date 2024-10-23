from gpiozero import Button
import os
import time
import logging
from pydub import AudioSegment
from pydub.playback import play
from datetime import datetime, timedelta
import os
from pydub import AudioSegment
from pydub.playback import play
# Other imports...

os.environ["SDL_AUDIODRIVER"] = "alsa"
os.environ["AUDIODEV"] = "hw:2,0"  # Set to your USB audio device

# Setup logging
logging.basicConfig(filename='/home/senior/audio_record.log', level=logging.DEBUG)

# GPIO setup
button1 = Button(17)  # Button 1 for recording
button2 = Button(22)  # Button 2 for playback (using GPIO pin 22)

# Directory to save recordings
recordings_dir = "/home/senior/recordings"
os.makedirs(recordings_dir, exist_ok=True)

file_count = 1
recording = False
recording_process = None
message_one_played = False

def start_recording():
    global recording_process, file_count
    try:
        wav_file_name = os.path.join(recordings_dir, f"recording_{file_count}.wav")
        # Start recording using arecord with mono channel and 22050 Hz
        recording_process = os.popen(f"arecord -D plughw:2,0 -f S16_LE -c1 -r 22050 {wav_file_name}")
        logging.info(f"Recording just started: {wav_file_name}")
        print(f"Recording just started: {wav_file_name}")
    except Exception as e:
        logging.exception("Failed to start recording")

def stop_recording():
    global recording_process, file_count
    try:
        if recording_process:
            # Stop recording
            os.system('pkill -f "arecord"')
            recording_process = None
            logging.info(f"Recording stopped. Saved as recording_{file_count}.wav")
            print(f"Recording stopped. Saved as recording_{file_count}.wav")
            file_count += 1  # Increment file count for the next recording
    except Exception as e:
        logging.exception("Failed to stop recording")

def play_audio(file_number):
    try:
        wav_file_name = os.path.join(recordings_dir, f"recording_{file_number}.wav")
        if os.path.exists(wav_file_name):
            print(f"Playing message {file_number}")
            # Use pydub to play the audio
            audio = AudioSegment.from_wav(wav_file_name)
            play(audio)
            logging.info(f"Playing {wav_file_name}")
        else:
            logging.warning(f"No WAV file found for recording {file_number}")
            print(f"No WAV file found for recording {file_number}")
    except Exception as e:
        logging.exception("Failed to play audio: {e}")

def handle_button1_press():
    global recording
    logging.info(f"Button 1 press detected. Current recording state: {recording}")

    if recording:
        logging.info("Stopping recording...")
        stop_recording()
        recording = False
    else:
        logging.info("Starting recording...")
        start_recording()
        recording = True

def handle_button2_press():
    press_count = 0
    press_time = time.time()

    while time.time() - press_time < 1.5:
        if button2.is_pressed:
            press_count += 1
            logging.info(f"Button 2 pressed {press_count} times")
            while button2.is_pressed:  # Wait until the button is released
                time.sleep(0.1)
            time.sleep(0.2)  # Debounce

    if press_count > 0:
        play_audio(press_count)

def check_time_for_message_one():
    global message_one_played
    current_time = datetime.now()
    target_time = current_time.replace(hour=19, minute=17, second=0, microsecond=0)

    # Check if it's 6:05 PM and the message hasn't been played yet
    if target_time <= current_time < (target_time + timedelta(minutes=1)) and not message_one_played:
        play_audio(1)
        message_one_played = True  # Prevent playing the message again

    # Reset for the next day
    if current_time >= (target_time + timedelta(minutes=1)):
        message_one_played = False

try:
    while True:
        if button1.is_pressed:
            handle_button1_press()
            while button1.is_pressed:  # Wait until the button is released
                time.sleep(0.1)
            time.sleep(0.2)  # Debounce

        if button2.is_pressed:
            handle_button2_press()
            while button2.is_pressed:  # Wait until the button is released
                time.sleep(0.1)
            time.sleep(0.2)  # Debounce

        check_time_for_message_one()
        time.sleep(1)  # Check every second for the time

except Exception as e:
    logging.error(f"An unexpected error occurred: {e}")
finally:
    if recording:
        stop_recording()
