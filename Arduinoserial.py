import serial
import time

PORT = "COM4"
BAUD = 19200
LINE_WIDTH = 40
ser = serial.Serial(PORT, BAUD, timeout=1)

lines = {
    "addr":"addr ?",
    "VAL":"VAL ?",
    "PLT_TEMP_AUTO_MAN": "PLT_TEMP_AUTO_MAN ?",
    "PLT_CABIN_PRESS_DUMP": "PLT_CABIN_PRESS_DUMP ?",
    "PLT_RAM_AIR": "PLT_RAM_AIR ?",
    "PLT_TEMP": "PLT_TEMP ?",
}

rate_line = "RATE: ? Hz"

last_time = None
rate = 0

def print_all():
    for key in lines:
        print(f"{lines[key]:<{LINE_WIDTH}}")
    print(f"{rate_line:<{LINE_WIDTH}}")

print_all()

try:
    while True:
        raw = ser.readline().decode(errors="ignore").strip()

        if not raw:
            continue

        # --- rate calculation ---
        now = time.time()
        if last_time is not None:
            dt = now - last_time
            if dt > 0:
                new_rate = 1.0 / dt

                # simple smoothing (optional, prevents jumping)
                rate = 0.8 * rate + 0.2 * new_rate

        last_time = now
        rate_line = f"RATE: {rate:6.1f} Hz"

        # --- parse input ---
        parts = raw.split()
        if len(parts) != 2:
            continue

        key, value = parts

        updated = False

        if key in lines:
            new_line = f"{key} {value}"

            if lines[key] != new_line:
                lines[key] = new_line
                updated = True

        # also update if rate changed significantly
        if updated:
            print(f"\033[{len(lines)+1}A", end="")  # +1 for rate line
            print_all()

except KeyboardInterrupt:
    print("\nExiting...")
finally:
    ser.close()