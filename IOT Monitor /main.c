/*
 * Project 3: Dynamic IoT Event Monitoring and Callback Engine
 * Student-style implementation using structures, unions, pointers and callbacks.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NAME_SIZE 40
#define TYPE_TEMPERATURE 1
#define TYPE_PRESSURE 2
#define TYPE_BATTERY 3
#define NORMAL 0
#define ALERT 1

typedef union {
    float temperature;
    int pressure;
    float voltage;
} Reading;

typedef struct {
    int id;
    char name[NAME_SIZE];
    int type;
    Reading reading;
    int status;
    int has_previous;
    float previous_value;
    int drift_alerts;
} Device;

typedef struct {
    Device *devices;
    size_t count;
    size_t capacity;
    unsigned long readings_processed;
    unsigned long normal_readings;
    unsigned long abnormal_readings;
    unsigned long alerts_by_type[4];
} Monitor;

typedef void (*DeviceCallback)(Device *);
typedef void (*CustomCallback)(Device *);

static float get_value(const Device *device);
static const char *type_name(int type);
static void display_device(const Device *device);
static int find_device_index(const Monitor *monitor, int id);
static int register_device(Monitor *monitor);
static void display_devices(const Monitor *monitor);
static void remove_device(Monitor *monitor);
static void process_device(Device *device, DeviceCallback callback);
static void temperature_monitor(Device *device);
static void pressure_monitor(Device *device);
static void battery_monitor(Device *device);
static void sensor_drift_detector(Device *device);
static void simulate_readings(Monitor *monitor);
static void display_alerts(const Monitor *monitor);
static void monitoring_summary(const Monitor *monitor);
static void cleanup(Monitor *monitor);
static void print_menu(void);
static void menu_register(Monitor *monitor);
static void menu_display(Monitor *monitor);
static void menu_remove(Monitor *monitor);
static void menu_simulate(Monitor *monitor);
static void menu_alerts(Monitor *monitor);
static void menu_summary(Monitor *monitor);
static void menu_exit(Monitor *monitor);
static int read_int(const char *prompt);

static float get_value(const Device *device)
{
    switch (device->type) {
        case TYPE_TEMPERATURE: return device->reading.temperature;
        case TYPE_PRESSURE: return (float)device->reading.pressure;
        case TYPE_BATTERY: return device->reading.voltage;
        default: return 0.0f;
    }
}

static const char *type_name(int type)
{
    switch (type) {
        case TYPE_TEMPERATURE: return "Temperature";
        case TYPE_PRESSURE: return "Pressure";
        case TYPE_BATTERY: return "Battery";
        default: return "Unknown";
    }
}

static void display_device(const Device *device)
{
    printf("ID: %d | Name: %-18s | Type: %-11s | ",
           device->id, device->name, type_name(device->type));

    switch (device->type) {
        case TYPE_TEMPERATURE:
            printf("Reading: %6.2f C", device->reading.temperature);
            break;
        case TYPE_PRESSURE:
            printf("Reading: %6d kPa", device->reading.pressure);
            break;
        case TYPE_BATTERY:
            printf("Reading: %6.2f V", device->reading.voltage);
            break;
        default:
            printf("Reading: --");
    }

    printf(" | Status: %s\n", device->status == ALERT ? "ALERT" : "NORMAL");
}

static int find_device_index(const Monitor *monitor, int id)
{
    size_t i;
    const Device *ptr = monitor->devices;

    for (i = 0; i < monitor->count; i++, ptr++) {
        if (ptr->id == id) return (int)i;
    }
    return -1;
}

static int register_device(Monitor *monitor)
{
    Device new_device;
    int type;

    printf("\n--- Register a device ---\n");
    new_device.id = read_int("Enter device ID: ");
    if (find_device_index(monitor, new_device.id) != -1) {
        printf("That ID is already in use. Device was not added.\n");
        return 0;
    }

    printf("Device name: ");
    if (scanf(" %39[^\n]", new_device.name) != 1) {
        printf("Invalid name.\n");
        return 0;
    }

    printf("Type: 1. Temperature  2. Pressure  3. Battery\n");
    type = read_int("Choose type: ");
    if (type < TYPE_TEMPERATURE || type > TYPE_BATTERY) {
        printf("Invalid device type.\n");
        return 0;
    }

    new_device.type = type;
    new_device.status = NORMAL;
    new_device.has_previous = 0;
    new_device.previous_value = 0.0f;
    new_device.drift_alerts = 0;
    new_device.reading.temperature = 0.0f;

    if (monitor->count == monitor->capacity) {
        size_t new_capacity = monitor->capacity == 0 ? 2 : monitor->capacity * 2;
        Device *temp = realloc(monitor->devices, new_capacity * sizeof(Device));
        if (temp == NULL) {
            printf("Could not allocate memory. Device was not added.\n");
            return 0;
        }
        monitor->devices = temp;
        monitor->capacity = new_capacity;
    }

    monitor->devices[monitor->count] = new_device;
    monitor->count++;
    printf("Device %s (ID %d) registered.\n", new_device.name, new_device.id);
    return 1;
}

static void display_devices(const Monitor *monitor)
{
    const Device *ptr = monitor->devices;
    size_t i;

    printf("\n--- Registered devices (%zu) ---\n", monitor->count);
    if (monitor->count == 0) {
        printf("No devices are registered.\n");
        return;
    }

    for (i = 0; i < monitor->count; i++, ptr++) display_device(ptr);
}

static void remove_device(Monitor *monitor)
{
    int id = read_int("Enter the ID of the device to remove: ");
    int index = find_device_index(monitor, id);
    size_t i;

    if (index == -1) {
        printf("No device with ID %d was found.\n", id);
        return;
    }

    printf("Removing %s (ID %d).\n", monitor->devices[index].name, id);
    for (i = (size_t)index; i + 1 < monitor->count; i++) {
        monitor->devices[i] = monitor->devices[i + 1];
    }
    monitor->count--;

    if (monitor->count == 0) {
        free(monitor->devices);
        monitor->devices = NULL;
        monitor->capacity = 0;
    } else if (monitor->capacity > 2 && monitor->count <= monitor->capacity / 4) {
        size_t new_capacity = monitor->capacity / 2;
        Device *temp = realloc(monitor->devices, new_capacity * sizeof(Device));
        if (temp != NULL) {
            monitor->devices = temp;
            monitor->capacity = new_capacity;
        }
    }
    printf("Device removed successfully.\n");
}

static void temperature_monitor(Device *device)
{
    float value = device->reading.temperature;
    if (value < 0.0f || value > 40.0f) {
        device->status = ALERT;
        printf("  [temperature_monitor] ALERT: %.2f C is outside 0.00 to 40.00 C.\n", value);
    } else {
        device->status = NORMAL;
        printf("  [temperature_monitor] Normal: temperature is within 0.00 to 40.00 C.\n");
    }
}

static void pressure_monitor(Device *device)
{
    int value = device->reading.pressure;
    if (value < 90 || value > 110) {
        device->status = ALERT;
        printf("  [pressure_monitor] ALERT: %d kPa is outside 90 to 110 kPa.\n", value);
    } else {
        device->status = NORMAL;
        printf("  [pressure_monitor] Normal: pressure is within 90 to 110 kPa.\n");
    }
}

static void battery_monitor(Device *device)
{
    float value = device->reading.voltage;
    if (value < 3.3f || value > 4.2f) {
        device->status = ALERT;
        printf("  [battery_monitor] ALERT: %.2f V is outside 3.30 to 4.20 V.\n", value);
    } else {
        device->status = NORMAL;
        printf("  [battery_monitor] Normal: voltage is within 3.30 to 4.20 V.\n");
    }
}

/* Custom callback:
 * It compares the current reading with the previous reading for that device.
 * A change greater than 8 C, 15 kPa or 0.35 V is treated as possible sensor drift.
 * The comparison uses absolute difference, and the previous value is then updated.
 */
static void sensor_drift_detector(Device *device)
{
    float current = get_value(device);
    float limit;

    if (!device->has_previous) {
        printf("  [sensor_drift_detector] First reading saved; no previous value to compare.\n");
        device->previous_value = current;
        device->has_previous = 1;
        return;
    }

    if (device->type == TYPE_TEMPERATURE) limit = 8.0f;
    else if (device->type == TYPE_PRESSURE) limit = 15.0f;
    else limit = 0.35f;

    {
        float difference = current - device->previous_value;
        if (difference < 0.0f) difference = -difference;

        if (difference > limit) {
            device->drift_alerts++;
            printf("  [sensor_drift_detector] DRIFT WARNING: change %.2f exceeds %.2f.\n",
                   difference, limit);
        } else {
            printf("  [sensor_drift_detector] Stable change: %.2f (limit %.2f).\n",
                   difference, limit);
        }
    }

    device->previous_value = current;
}

static void process_device(Device *device, DeviceCallback callback)
{
    if (callback != NULL) {
        callback(device);
    }
}

static void simulate_readings(Monitor *monitor)
{
    size_t i;
    int number;

    if (monitor->count == 0) {
        printf("Register at least one device before simulating readings.\n");
        return;
    }

    number = read_int("How many readings should be generated (minimum 10): ");
    if (number < 10) {
        printf("The assignment requires at least 10 readings. Using 10.\n");
        number = 10;
    }

    printf("\n--- Simulated readings ---\n");
    for (i = 0; i < (size_t)number; i++) {
        Device *device = &monitor->devices[i % monitor->count];
        DeviceCallback callback = NULL;
        CustomCallback extra_callback = sensor_drift_detector;

        switch (device->type) {
            case TYPE_TEMPERATURE:
                callback = temperature_monitor;
                device->reading.temperature = -5.0f +
                    (float)rand() / (float)RAND_MAX * 55.0f; /* -5 to 50 C */
                break;
            case TYPE_PRESSURE:
                callback = pressure_monitor;
                device->reading.pressure = 80 + rand() % 51; /* 80 to 130 kPa */
                break;
            case TYPE_BATTERY:
                callback = battery_monitor;
                device->reading.voltage = 3.0f +
                    (float)rand() / (float)RAND_MAX * 1.5f; /* 3.0 to 4.5 V */
                break;
            default:
                printf("Unknown device type; skipping.\n");
                continue;
        }

        printf("\nReading %zu:\n", i + 1);
        printf("  Device ID: %d | Name: %s | Type: %s | Value: ",
               device->id, device->name, type_name(device->type));
        if (device->type == TYPE_TEMPERATURE)
            printf("%.2f C\n", device->reading.temperature);
        else if (device->type == TYPE_PRESSURE)
            printf("%d kPa\n", device->reading.pressure);
        else
            printf("%.2f V\n", device->reading.voltage);

        process_device(device, callback);
        process_device(device, extra_callback);

        monitor->readings_processed++;
        if (device->status == ALERT) {
            monitor->abnormal_readings++;
            monitor->alerts_by_type[device->type]++;
        } else {
            monitor->normal_readings++;
        }
    }
    printf("\nFinished processing %d readings.\n", number);
}

static void display_alerts(const Monitor *monitor)
{
    size_t i;
    int found = 0;

    printf("\n--- Devices currently in alert status ---\n");
    for (i = 0; i < monitor->count; i++) {
        const Device *device = &monitor->devices[i];
        if (device->status == ALERT) {
            found = 1;
            display_device(device);
            if (device->type == TYPE_TEMPERATURE)
                printf("  Expected range: 0 to 40 C | Callback: temperature_monitor\n");
            else if (device->type == TYPE_PRESSURE)
                printf("  Expected range: 90 to 110 kPa | Callback: pressure_monitor\n");
            else
                printf("  Expected range: 3.30 to 4.20 V | Callback: battery_monitor\n");
        }
    }
    if (!found) printf("There are no devices currently in alert status.\n");
}

static void monitoring_summary(const Monitor *monitor)
{
    printf("\n========== MONITORING SUMMARY ==========\n");
    printf("Registered devices:       %zu\n", monitor->count);
    printf("Readings processed:       %lu\n", monitor->readings_processed);
    printf("Normal readings:          %lu\n", monitor->normal_readings);
    printf("Abnormal readings:        %lu\n", monitor->abnormal_readings);
    printf("Temperature alerts:       %lu\n", monitor->alerts_by_type[TYPE_TEMPERATURE]);
    printf("Pressure alerts:          %lu\n", monitor->alerts_by_type[TYPE_PRESSURE]);
    printf("Battery alerts:           %lu\n", monitor->alerts_by_type[TYPE_BATTERY]);

    {
        unsigned long total_drift = 0;
        size_t i;
        for (i = 0; i < monitor->count; i++)
            total_drift += (unsigned long)monitor->devices[i].drift_alerts;
        printf("Custom sensor drift warnings: %lu\n", total_drift);
    }
    printf("========================================\n");
}

static void cleanup(Monitor *monitor)
{
    free(monitor->devices);
    monitor->devices = NULL;
    monitor->count = 0;
    monitor->capacity = 0;
}

static int read_int(const char *prompt)
{
    int value;
    int result;
    int ch;

    printf("%s", prompt);
    result = scanf("%d", &value);
    while ((ch = getchar()) != '\n' && ch != EOF) {
        /* Clear the rest of the input line. */
    }
    if (result != 1) {
        printf("Invalid input. Using 0.\n");
        return 0;
    }
    return value;
}

static void print_menu(void)
{
    printf("\n===== DYNAMIC IoT EVENT MONITOR =====\n");
    printf("1. Register a device\n");
    printf("2. Display all devices\n");
    printf("3. Remove a device by ID\n");
    printf("4. Generate and process readings\n");
    printf("5. Display detected alerts\n");
    printf("6. Monitoring summary\n");
    printf("7. Exit\n");
}

static void menu_register(Monitor *monitor) { (void)register_device(monitor); }
static void menu_display(Monitor *monitor) { display_devices(monitor); }
static void menu_remove(Monitor *monitor) { remove_device(monitor); }
static void menu_simulate(Monitor *monitor) { simulate_readings(monitor); }
static void menu_alerts(Monitor *monitor) { display_alerts(monitor); }
static void menu_summary(Monitor *monitor) { monitoring_summary(monitor); }
static void menu_exit(Monitor *monitor) { (void)monitor; printf("Closing monitor...\n"); }

int main(void)
{
    Monitor monitor = {NULL, 0, 0, 0, 0, 0, {0, 0, 0, 0}};
    void (*menu_actions[7])(Monitor *) = {
        menu_register, menu_display, menu_remove, menu_simulate,
        menu_alerts, menu_summary, menu_exit
    };
    int choice;

    srand((unsigned int)time(NULL));
    printf("Dynamic IoT Event Monitoring and Callback Engine\n");
    printf("Operating ranges: temperature 0-40 C, pressure 90-110 kPa, battery 3.30-4.20 V.\n");
    printf("Simulation ranges: temperature -5 to 50 C, pressure 80 to 130 kPa, battery 3.0 to 4.5 V.\n");

    do {
        print_menu();
        choice = read_int("Choose an option: ");
        if (choice >= 1 && choice <= 7) {
            menu_actions[choice - 1](&monitor);
        } else {
            printf("Please choose a number from 1 to 7.\n");
        }
    } while (choice != 7);

    cleanup(&monitor);
    printf("Dynamic memory released. Goodbye.\n");
    return 0;
}
