import Vue from 'vue';
import Vuetify from 'vuetify';
Vue.config.productionTip = false;
Vue.use(Vuetify);

import 'jest-canvas-mock';
import fetchMock from 'jest-fetch-mock';
fetchMock.enableMocks();
