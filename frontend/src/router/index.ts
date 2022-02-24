import Vue from 'vue';
import VueRouter, {RouteConfig} from 'vue-router';
import Home from '@/views/home.vue';
import Mission from '@/views/mission.vue';

Vue.use(VueRouter);

export const HOME_ROUTE = '/';
export const MISSION_ROUTE = '/Mission';

const ROUTES: Array<RouteConfig> = [
  {
    path: HOME_ROUTE,
    name: 'Home',
    component: Home,
  },
  {
    path: MISSION_ROUTE,
    name: 'Mission',
    component: Mission,
  },
  {
    path: '*',
    redirect: '/',
  },
];

export const ROUTER = new VueRouter({
  mode: 'history',
  base: process.env.BASE_URL,
  routes: ROUTES,
});
