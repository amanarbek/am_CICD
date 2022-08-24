#!/bin/bash

TELEGRAM_BOT_TOKEN=5418128309:AAH3ldcOIZ9I_z8ufvc1HS4P0muE2IXRoVg
TELEGRAM_USER_ID=425477906
TIME=2

URL="https://api.telegram.org/bot$TELEGRAM_BOT_TOKEN/sendMessage"
TEXT="Deploy status: $1%0A%0AProject:+$CI_PROJECT_NAME%0AURL:+$CI_PROJECT_URL/pipelines/$CI_PIPELINE_ID/%0ABranch:+$CI_COMMIT_REF_SLUG"

curl -s --max-time $TIME -d "chat_id=$TELEGRAM_USER_ID&disable_web_page_preview=1&text=$TEXT" $URL > /dev/null
