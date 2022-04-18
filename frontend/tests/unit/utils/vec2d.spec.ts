import {Vec2d} from '@/utils/vec2d';

describe('Vec2d.ts', () => {
  it('should create', () => {
    const VEC = new Vec2d(1, 1);
    expect(VEC).toBeTruthy();
    expect(VEC.x).toEqual(1);
    expect(VEC.y).toEqual(1);
  });
});
