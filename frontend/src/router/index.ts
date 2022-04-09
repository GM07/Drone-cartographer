import Vue from 'vue';
import VueRouter, {RouteConfig} from 'vue-router';
import Home from '@/views/home.vue';
import Mission from '@/views/mission.vue';
import CompletedMissions from '@/views/completed_missions.vue';

Vue.use(VueRouter);

export const HOME_ROUTE = '/';
export const MISSION_ROUTE = '/Mission';
export const COMPLETED_ROUTE = '/History';
export const EDITOR_ROUTE = '/Editor';

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
    path: COMPLETED_ROUTE,
    name: 'CompletedMissions',
    component: CompletedMissions,
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
