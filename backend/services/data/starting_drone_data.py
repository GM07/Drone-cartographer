from typing import Any, Dict


class StartingDroneData:

    def __init__(self, drone_data: Dict[str, Any]):
        self.name: str = drone_data['name']
        self.starting_x_pos = drone_data['startingXPos']
        self.starting_y_pos = drone_data['startingYPos']
        self.starting_orientation = drone_data['startingOrientation']

    def __str__(self):
        return 'name: ' \
            + self.name \
            + ' starting_x_pos: ' \
            + str(self.starting_x_pos) \
            + ' starting_y_pos: ' \
            + str(self.starting_y_pos) \
            + ' starting_orientation: ' \
            + str(self.starting_orientation)
