<template>
  <v-dialog v-model="isDroneMenuOpen" max-width="390" persistent>
    <v-form ref="form">
      <v-card>
        <v-card-title>
          <span class="text-h5 justify-center">Nouveau Drone</span>
        </v-card-title>
        <v-card-text>
          <v-container>
            <v-row>
              <v-col cols="12">
                <v-text-field
                  v-model="newDrone.name"
                  label="Nom du drone"
                  required
                  :rules="[validateDroneExistTwice, mandatory]"
                ></v-text-field>
              </v-col>
            </v-row>
            <v-form ref="pos">
              <v-row>
                <v-col cols="12">
                  <v-text-field
                    v-model="newDrone.xPos"
                    label="Position X"
                    :rules="[
                      validateDistance,
                      validatePosition,
                      v =>
                        !isNaN(parseFloat(Number(v))) ||
                        'Le nombre entré n\'est pas valide',
                    ]"
                    @change="validatePositionForm()"
                  ></v-text-field>
                </v-col>
              </v-row>
              <v-row>
                <v-col cols="12">
                  <v-text-field
                    v-model="newDrone.yPos"
                    label="Position Y"
                    :rules="[
                      validateDistance,
                      validatePosition,
                      v =>
                        !isNaN(parseFloat(Number(v))) ||
                        'Le nombre entré n\'est pas valide',
                    ]"
                    @change="validatePositionForm()"
                  ></v-text-field>
                </v-col>
              </v-row>
            </v-form>
          </v-container>
        </v-card-text>
        <v-card-actions>
          <v-spacer></v-spacer>
          <v-btn color="blue darken-1" text @click="closeMenu"> Fermer </v-btn>
          <v-btn color="blue darken-1" text @click="submit">
            Enregistrer
          </v-btn>
        </v-card-actions>
      </v-card>
    </v-form>
  </v-dialog>
</template>

<script lang="ts">
import {Component, Vue, Prop} from 'vue-property-decorator';
import {Drone} from '@/communication/drone';

@Component({})
export default class DroneMenu extends Vue {
  @Prop() private droneList!: Drone[];
  @Prop() private isDroneMenuOpen!: boolean;
  private newDrone: Drone = {
    name: 'radio://0/80/2M/E7E7E7E761',
    xPos: 0,
    yPos: 0,
  };

  public submit(): void {
    if (
      (this.$refs.form as Vue & {validate: () => boolean}).validate() &&
      (this.$refs.pos as Vue & {validate: () => boolean}).validate()
    ) {
      this.$emit('addDrone', this.newDrone);
      this.newDrone.name = 'radio://0/80/2M/E7E7E7E761';
      this.newDrone.xPos = 0;
      this.newDrone.yPos = 0;
      this.resetValidation();
      this.closeMenu();
    }
  }

  public closeMenu(): void {
    this.$emit('setDroneMenuOpen', false);
  }

  public resetValidation(): void {
    (
      this.$refs.form as Vue & {resetValidation: () => boolean}
    ).resetValidation();
  }

  validatePositionForm(): void {
    (this.$refs.pos as Vue & {validate: () => boolean}).validate();
  }

  public validateDroneExistTwice(): boolean | string {
    return (
      this.droneList.filter(drone => {
        return drone.name === this.newDrone.name;
      }).length === 0 || 'Ce drone a déjà été créé'
    );
  }

  public validateDroneNameLength(v: string): boolean | string {
    return (
      v.length < 40 ||
      v.length === 0 ||
      'Le nom du drone doit être entre 0 et 40 caractères'
    );
  }

  public validateDistance(): boolean | string {
    if (this.newDrone.xPos === null || this.newDrone.yPos === null) {
      return true;
    }

    return (
      this.droneList.filter(drone => {
        return (
          Math.sqrt(
            Math.pow(drone.xPos - this.newDrone.xPos, 2) +
              Math.pow(drone.yPos - this.newDrone.yPos, 2)
          ) < 0.5
        );
      }).length === 0 ||
      'Un drone existant est trop près de la position choisie'
    );
  }

  public validatePosition(v: number): boolean | string {
    return (
      (v <= 2 && v >= -2) ||
      'La position du drone doit être dans les limites [-2, 2]'
    );
  }

  public validateDroneNameCharacters(v: string): boolean | string {
    return (
      v.match('^[a-zA-Z0-9_:/]*$') !== null ||
      'Le nom du drone contient des caractères invalides'
    );
  }

  public mandatory(v: unknown): boolean | string {
    return !!v || 'Ce champ est obligatoire';
  }
}
</script>
