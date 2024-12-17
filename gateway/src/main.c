#include <zephyr/kernel.h>
#include <zephyr/net/conn_mgr_connectivity.h>
#include <zephyr/net/conn_mgr_monitor.h>
#include <memfault/metrics/metrics.h>
#include <memfault/ports/zephyr/http.h>
#include <memfault/core/data_packetizer.h>
#include <dk_buttons_and_leds.h>

#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(memfault_sample, CONFIG_MEMFAULT_SAMPLE_LOG_LEVEL);

#define L4_EVENT_MASK          (NET_EVENT_L4_CONNECTED | NET_EVENT_L4_DISCONNECTED)
#define CONN_LAYER_EVENT_MASK  (NET_EVENT_CONN_IF_FATAL_ERROR)

static K_SEM_DEFINE(nw_connected_sem, 0, 1);
static struct net_mgmt_event_callback l4_cb;
static struct net_mgmt_event_callback conn_cb;

/* Retrieve device temperature mock function */
static uint32_t get_device_temperature(void) {
    return 25;  // Replace with sensor logic
}

static void update_device_metrics(void) {
    uint32_t temperature = get_device_temperature();
    MEMFAULT_METRIC_SET_UNSIGNED(device_temperature, temperature);

    LOG_INF("Device temperature updated: %u", temperature);
}

static void on_connect(void) {
    LOG_INF("Connected to network. Sending data to Memfault...");
    memfault_zephyr_port_post_data();
}

static void l4_event_handler(struct net_mgmt_event_callback *cb, uint32_t event, struct net_if *iface) {
    switch (event) {
    case NET_EVENT_L4_CONNECTED:
        LOG_INF("Network connectivity established");
        k_sem_give(&nw_connected_sem);
        break;
    case NET_EVENT_L4_DISCONNECTED:
        LOG_INF("Network connectivity lost");
        break;
    }
}

int main(void) {
    int err;
    LOG_INF("Memfault sample has started");

    net_mgmt_init_event_callback(&l4_cb, l4_event_handler, L4_EVENT_MASK);
    net_mgmt_add_event_callback(&l4_cb);

    LOG_INF("Connecting to network...");
    err = conn_mgr_all_if_up(true);
    if (err) {
        LOG_ERR("Failed to bring up interface");
        return err;
    }

    err = conn_mgr_all_if_connect(true);
    if (err) {
        LOG_ERR("Failed to connect to network");
        return err;
    }

    k_sem_take(&nw_connected_sem, K_FOREVER);

    while (1) {
        on_connect();
        update_device_metrics();
        k_sleep(K_SECONDS(60));
    }

    return 0;
}
