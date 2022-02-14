import unittest

from server import APP

class TestServer(unittest.TestCase):

    def test_get_drones(self):
        tester = APP.test_client(self)
        response = tester.get('/getDrones', content_type='html/text')
        self.assertEqual(response.status_code, 200)  
