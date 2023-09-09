import pandas as pd  
from sqlalchemy import create_engine
import re

# local instance for now, will connect to official one later
cnx_string = 'mysql+mysqlconnector://root:@localhost:3306/midnight_sun'

engine = create_engine(cnx_string)

# sample can msgs from https://uwmidsun.atlassian.net/wiki/spaces/ELEC/pages/3157950471/Meeting+Notes+-+Strategy+x+FW
# msg1 = {
#   "id": 0,
#   "source": "BMS_CARRIER",
#   "target": "CENTRE_CONSOLE",
#   "msg_name": "bps heartbeat",
#   "is_critical": True,
#   "can_data": {
#     "u8": {
#       "field_name_1": "status"
#     }
#   }
# }

# msg2 = {
#   "id": 1,
#   "source": "CENTRE_CONSOLE",
#   "target": "BMS_CARRIER, SOLAR_5_MPPTS, SOLAR_6_MPPTS, MOTOR_CONTROLLER",
#   "msg_name": "set relay states",
#   "is_critical": True,
#   "can_data": {
#     "u16": {
#       "field_name_1": "relay_mask",
#       "field_name_2": "relay_state"
#     }
#   }
# }

# msg3 = {
#   "id": 33,
#   "source": "BMS_CARRIER",
#   "target": "TELEMETRY",
#   "msg_name": "battery aggregate vc",
#   "msg_readable_name": "battery aggregate voltage and current",
#   "can_data": {
#     "u32": {
#       "field_name_1": "voltage",
#       "field_name_2": "current"
#     }
#   }
# }

# msg4 = {
#   "id": 55,
#   "source": "POWER_DISTRIBUTION_REAR",
#   "target": "CENTRE_CONSOLE",
#   "msg_name": "rear current measurement",
#   "can_data": {
#     "u16": {
#       "field_name_1": "current_id",
#       "field_name_2": "current"
#     }
#   }
# }

# array of sample can msgs for testing
# msgs = [msg1, msg2, msg3, msg4]

# non can_data -------
def non_can_data_to_mysql(msgs):

  df = pd.DataFrame(msgs, columns=["id", "source", "target", "msg_name", "is_critical", "msg_readable_name"])

  # send to mysql db
  df.to_sql('non_can_data', con=engine, if_exists='append', index=False)

def can_data_to_mysql(msgs):
    # array to hold field values
    can_data_values = []
    # loop can msgs
    for msg in msgs:
        for key, value in msg["can_data"].items():
            # Extract u{x} value from key using regular expression
            match = re.search(r"u(\d+)", key)
            u_value = int(match.group(1))

            # loop through each nested field values and add it to the can_data array
            can_data_values.extend([
                {"id": msg["id"], "field_name": field_key, "field_value": field_value, "u_value": u_value}
                for field_key, field_value in value.items()
            ])

    df = pd.DataFrame(can_data_values)
    df.to_sql('can_data', con=engine, if_exists='append', index=False)

engine.dispose()