# Openstore manifest with daemon service

All thats required for the service desktop file is an `Exec=` line

```json
{
    "description": "Gear Up your Home",
    "framework": "ubuntu-sdk-15.04",
    "architecture": "@CLICK_ARCH@",
    "hooks": {
        "app": {
            "apparmor": "gup.apparmor",
            "desktop": "gup.desktop"
        },
        "service": {
            "desktop": "server/guhd.desktop"
        }
    },
    "maintainer": "Guh <guh@guh.guru>",
    "name": "guh.guru",
    "title": "guh",
    "version": "0.1.0"
}
```
