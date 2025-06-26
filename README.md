# Shared uWSGI Configurations and Plugins

This repository provides shared base configurations and plugins for multiple uWSGI instances.  
The primary goal is to enable consistent JSON logging from uWSGI, which requires a custom plugin to properly handle JSON escaping.

---

## 1. Building the `escape_json` Plugin

### 1.1. Integrate Directly into the Target Dockerfile

To compile the `escape_json` plugin and include it in your Docker image, add the following to your `Dockerfile`:

```dockerfile
RUN uwsgi --build-plugin /app/uwsgi-common/escape_json.c && \
    mv ./escape_json_plugin.so /app/uwsgi-common/
```

This command compiles the `escape_json.c` source file into a `escape_json_plugin.so` shared object and moves it to the `/app/uwsgi-common/` directory.

---

### 1.2. Build a Custom Base Image

You can also use the Dockerfile in this repository to build a custom base image for your uWSGI application.  
It accepts a `BUILD_FROM` argument to specify the base image. If not provided, it defaults to: `registry.access.redhat.com/ubi9/python-311`

---

## 2. Configuring uWSGI with `uwsgi.ini`

To use the shared configuration and plugin, update your `uwsgi.ini` file:

```ini
[uwsgi]
include = /app/uwsgi-common/uwsgi-base.ini
plugins = /app/uwsgi-common/escape_json_plugin.so
```

### Note

If you're using multiple plugins, list them comma-separated:

```ini
plugins = plugin1,plugin2,/app/uwsgi-common/escape_json_plugin.so
```

---
