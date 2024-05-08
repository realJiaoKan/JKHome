# MQTT Topic Naming Convention (For Reference Only)

## Overview

- Use UTF-8 encoding
- Only lowercase letters, numbers, underscores, dashes, and slashes (used only for hierarchical division) are allowed
- Maximum of 65535 characters allowed
- Cannot be empty
- Names should be specific and based on hierarchical divisions

## Hierarchical Division

Follow the convention of using slashes ('/') for hierarchical division, starting directly with the name of the first layer.

Name according to the format "geographical location/device type/device name/specific item".

### Geographical Location

Follow the order from largest to smallest: "country, province, city, district, specific name" for hierarchical division, with similar logic applied to peer administrative divisions.

e.g., "fra/lyon/3eme/home"

### Device Type

Refer to the "Matter Device Library Specification Version 1.1" (hereinafter referred to as the "Matter Standard") for the classification of smart devices, typically starting from "Chapter 4. Lighting Device Types". Hierarchical divisions can be made according to the sections in the "Matter Standard".

e.g., "lighting/onoff", "sensor/temperature"

### Device Name

No uniform naming convention is set; it depends on specific circumstances.