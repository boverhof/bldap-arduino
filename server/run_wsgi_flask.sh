#!/bin/bash
WORKERS=$1
APP="bldap_flask_app:app"
gunicorn -w  $WORKERS  -b :8000 $APP
