"""This module tests the functions implemented to communicate with the mongoDb.

It is called when you run pylint from the root. It contains unit test methods
and implements Mongomock
"""
from services.communication.database.mongo_interface import Database
import mongomock
import unittest
from unittest.mock import patch


def populate_db(db):
    for x in range(3):
        db.missions.insert_one({
            '_id': x,
            'timeStamp': 'test' + str(x),
            'name': 'name' + str(x)
        })


class TestApplication(unittest.TestCase):
    """This class has methods that tests all the methods implemented from the
    Database class present in
    services/communication/database/mongo_interface.py"""

    @patch('services.communication.database.mongo_interface.MongoClient',
           mongomock.MongoClient)
    def test_initialization(self):
        database = Database()
        self.assertTrue(isinstance(database.client, mongomock.MongoClient))

    @patch('services.communication.database.mongo_interface.MongoClient',
           mongomock.MongoClient)
    def test_upload_mission_info(self):
        database = Database()
        expected_value = {'name': 'test'}
        self.assertTrue(len(list(database.db.missions.find())) == 0)
        database.upload_mission_info({'name': 'test'})
        result = database.db.missions.find_one({'name': 'test'})
        self.assertEqual(expected_value['name'], result['name'])

    @patch('services.communication.database.mongo_interface.MongoClient',
           mongomock.MongoClient)
    def test_get_all_missions_time_stamps(self):
        database = Database()
        populate_db(database.db)
        expected_value = []
        for x in range(3):
            expected_value.append({'_id': str(x), 'timeStamp': 'test' + str(x)})
        result = database.get_all_missions_time_stamps()
        self.assertEqual(result, expected_value)

    @patch('services.communication.database.mongo_interface.MongoClient',
           mongomock.MongoClient)
    def test_get_missions_from_name(self):
        database = Database()
        populate_db(database.db)
        expected_value = [{'_id': '1', 'timeStamp': 'test1', 'name': 'name1'}]
        result = database.get_missions_from_name('name1')
        self.assertEqual(result, expected_value)

    @patch('services.communication.database.mongo_interface.MongoClient',
           mongomock.MongoClient)
    def test_get_mission_from_id(self):
        database = Database()
        populate_db(database.db)
        expected_value = {'_id': 1, 'timeStamp': 'test1', 'name': 'name1'}
        result = database.get_mission_from_id(1)
        self.assertEqual(result, expected_value)

    @patch('services.communication.database.mongo_interface.MongoClient',
           mongomock.MongoClient)
    def test_remove_mission_from_id(self):
        database = Database()
        populate_db(database.db)
        self.assertTrue(database.remove_mission_from_id(1))
        result = database.get_mission_from_id(1)
        self.assertEqual(result, None)

    @patch('services.communication.database.mongo_interface.MongoClient',
           mongomock.MongoClient)
    def test_update_mission_info_from_id(self):
        database = Database()
        populate_db(database.db)
        expected_value = {'_id': 1, 'name': 'new', 'timeStamp': 'newTs'}
        self.assertFalse(database.update_mission_info_from_id({'test': 1}))
        result = database.update_mission_info_from_id(expected_value)
        self.assertEqual(result.modified_count, 1)
        self.assertEqual(expected_value,
                         database.db.missions.find_one({'_id': 1}))
