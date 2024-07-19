from faker import Faker
import json
import argparse

def generate_address(faker: Faker) -> dict:
  addr: dict = {
    "street_address": faker.street_address(),
    "city": faker.city(),
    "state": faker.state_abbr(),
    "postcode": faker.postcode(),
    "country_code": faker.current_country_code()
  }
  return addr

parser = argparse.ArgumentParser(description='Generate n amount of users')
parser.add_argument('number', metavar='n', type=int, nargs='+', help='Number of users to generate')
args = parser.parse_args()
num_users = args.number[0]
print('Generating', num_users, 'users...')

# help(type(Faker))
fake = Faker('en_AU')
users_data: list = []

for i in range(num_users):
  first_name: str = fake.first_name()
  last_name: str = fake.last_name()
  address: dict = generate_address(fake)
  phone_number: str = fake.phone_number().replace(".", "").replace("-", "").replace(" ", "").replace("(", "").replace(")", "")
  email: str = first_name.lower() + last_name.lower() + "@streemer.com"

  new_user_details: dict = {
    "first_name": first_name,
    "last_name": last_name,
    "address": address,
    "contact_number": phone_number,
    "email": email
  }
  users_data.append(new_user_details)

users_json: dict = {
  "users": users_data
}

with open('users.json', 'w') as users_file:
  json.dump(users_json, users_file, indent=2, sort_keys=True)
