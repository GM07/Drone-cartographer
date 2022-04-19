export class TreeNodeStub {
  public id: string;
  public label: string;
  public children: TreeNodeStub[] | undefined;

  constructor(id: string, label: string, children?: TreeNodeStub[]) {
    this.id = id;
    this.label = label;
    this.children = children;
  }

  clone(): TreeNodeStub {
    const CHILDREN = this.children?.map(node => node.clone());
    return new TreeNodeStub(this.id, this.label, CHILDREN);
  }
}
