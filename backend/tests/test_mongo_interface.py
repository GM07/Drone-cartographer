"""This module tests the functions implemented to communicate with the mongoDb.

It is called when you run pylint from the root. It contains unit test methods
and implements Mongomock
"""
from services.communication.database.mongo_interface import Database, Mission
import mongomock
import unittest
from unittest.mock import patch


def populate_db(db):
    for x in range(3):
        db.missions.insert_one(
        {'_id':x,'time_stamp': str(4+x),'temps_de_vol': 10+x,
        'nombres_de_drones': 3+x,'est_simule': False,'distance_totale': 40+x,
        'cartes': [[{'x':1+x,'y':2+x}]]})


class TestApplication(unittest.TestCase):
    """This class has methods that tests all the methods implemented from the
    Database class present in
    services/communication/database/mongo_interface.py"""
    @patch('services.communication.database.mongo_interface.MongoClient'
    , mongomock.MongoClient)
    def test_initialization(self):
        database = Database()
        self.assertTrue(isinstance(database.client, mongomock.MongoClient))

    @patch('services.communication.database.mongo_interface.MongoClient'
    , mongomock.MongoClient)
    def test_upload_mission_info(self):
        database = Database()
        test = Mission('4',10,3,False,40,[[{'x':1,'y':2}]])
        self.assertTrue(len(list(database.db.missions.find())) == 0)
        database.upload_mission_info(test)
        result = database.db.missions.find_one({'distance_totale': 40})
        self.assertEqual(test.distance_totale, result['distance_totale'])

    @patch('services.communication.database.mongo_interface.MongoClient'
    , mongomock.MongoClient)
    def test_get_all_missions_time_stamps(self):
        database = Database()
        populate_db(database.db)
        expected_value = []
        for x in range(3):
            expected_value.append(
                {'_id': str(x), 'time_stamp': str(4+x) , 'est_simule': False})
        result = database.get_all_missions_time_stamps()
        self.assertEqual(result, expected_value)

    @patch('services.communication.database.mongo_interface.MongoClient'
    , mongomock.MongoClient)
    def test_get_mission_from_id(self):
        database = Database()
        populate_db(database.db)
        expected_value =  {'_id':2,'time_stamp': str(4+2),'temps_de_vol': 10+2,
        'nombres_de_drones': 3+2,'est_simule': False,'distance_totale': 40+2,
        'cartes': [[{'x':1+2,'y':2+2}]]}
        result = database.get_mission_from_id(2)
        self.assertEqual(result, expected_value)

    @patch('services.communication.database.mongo_interface.MongoClient'
    , mongomock.MongoClient)
    def test_remove_mission_from_id(self):
        database = Database()
        populate_db(database.db)
        self.assertTrue(database.remove_mission_from_id(1))
        result = database.get_mission_from_id(1)
        self.assertEqual(result, None)

    @patch('services.communication.database.mongo_interface.MongoClient'
    , mongomock.MongoClient)
    def test_update_mission_info_from_id(self):
        database = Database()
        populate_db(database.db)
        test = Mission('8',10,3,False,40,[[{'x':1,'y':2}]])
        self.assertFalse(database.update_mission_info_from_id(test,5))
        self.assertTrue(database.update_mission_info_from_id(test,2))
        self.assertEqual(test.time_stamp,
        database.db.missions.find_one({'_id':2})['time_stamp'])
