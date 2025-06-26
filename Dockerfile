ARG BUILD_FROM=registry.access.redhat.com/ubi9/python-311
FROM $BUILD_FROM

USER root

RUN yum --disableplugin subscription-manager -y update && \
    yum --disableplugin subscription-manager -y --allowerasing update && \
    yum --disableplugin subscription-manager -y install pcre-devel nmap-ncat && \
    yum --disableplugin subscription-manager -y clean all

RUN mkdir /app && chown -R 1000:0 /app

WORKDIR /app

# Only whitelisted files, see: .dockerignore
COPY --chown=1000:0 . /app/

USER 1000

RUN pip install -U pip && \
    pip install --no-cache-dir -r /app/requirements.txt && \
    uwsgi --build-plugin /app/escape_json.c
