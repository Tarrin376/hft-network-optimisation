import pandas as pd
from pyvis.network import Network
import os

def visualize_network(nodes_csv, edges_csv, requests_csv, answer_csv, output_html="network_viz.html"):
    base_path = os.path.dirname(__file__)
    nodes_csv = os.path.join(base_path, "..", "data_files", f"{nodes_csv}.csv")
    edges_csv = os.path.join(base_path, "..", "data_files", f"{edges_csv}.csv")
    requests_csv = os.path.join(base_path, "..", "data_files", f"{requests_csv}.csv")
    answer_csv = os.path.join(base_path, "..", "data_files", f"{answer_csv}.csv")

    nodes_df = pd.read_csv(nodes_csv)
    edges_df = pd.read_csv(edges_csv)
    requests_df = pd.read_csv(requests_csv)
    answer_df = pd.read_csv(answer_csv)

    net = Network(height="80vh", width="100%", directed=True)
    solution_set = set(zip(answer_df['From'], answer_df['To']))

    for _, row in nodes_df.iterrows():
        id, is_server = int(row['NodeId']), bool(row['IsServer'])
        net.add_node(id, label=f"Node {id}", color="#83a0ff" if is_server else "#ffa1a9")

    for _, row in edges_df.iterrows():
        u, v = int(row['Source']), int(row['Dest'])
        
        if (u, v) in solution_set:
            color = "#ff4d4d"
            width = 5
            label = "Selected"
        else:
            color = "#444444"
            width = 1
            label = ""

        net.add_edge(u, v, color=color, width=width, title=label)

    for _, row in requests_df.iterrows():
        src, dest = int(row['Server']), int(row['Exchange'])

        if src in net.node_ids:
            net.get_node(src)['color'] = "#00ff6a"
            net.get_node(src)['size'] = 25
        if dest in net.node_ids:
            net.get_node(dest)['color'] = "#ff00ff"
            net.get_node(dest)['size'] = 25

    net.barnes_hut(gravity=-8000, central_gravity=0.3, spring_length=150)
    net.show_buttons(filter_=['physics'])
    net.write_html(output_html)

if __name__ == "__main__":
    visualize_network(
        nodes_csv="NODES_4",
        edges_csv="EDGES_4",
        requests_csv="REQUESTS_4",
        answer_csv="ANS_4"
    )